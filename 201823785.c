#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "sensor.h"


int serv_sock,clnt_sock=-1;
struct sockaddr_in serv_addr,clnt_addr;
socklen_t clnt_addr_size;
char buffer[4096];
int fd = 0;


void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

void *accept_client(void* status){
    
    int rd =0;

    int serv2_sock = dup(serv_sock);
    int fd2 = dup(fd);

    if(clnt_sock<0){           
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv2_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
            error_handling("accept() error");
        //client요청이 연결허용되었을때
        else{
            //log.txt에 쓰여진 data read
            printf("success\n");
            fd2 = open("log.txt", O_RDONLY, 0777);
            rd = read(fd2, buffer, sizeof(buffer));
            
            if(rd == -1){
                error_handling("read fail");
            }
            else{
                if(write(clnt_sock, buffer, sizeof(buffer))!= -1){
                    memset(buffer, 0 , sizeof(buffer));
                    //eof?
                    close(clnt_sock);
                    //clnt_sock의 스트림을 닫은 후 다시 clnt_sock 초기화
                    clnt_sock = -1;
                    int *state = (int*)status;
                    *state = 1;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {

    char buffer[256];
    int fd = 0;
    mode_t mode = 0666;
    DATA* sensordata;
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");
        
    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock,5) == -1)
            error_handling("listen() error");
    
    fd = open("log.txt", O_WRONLY|O_CREAT|O_EXCL, mode);
    if(fd == -1){
        perror("failed open");
        return 1;
    }

    pthread_t p_thread[1];
    int thr_id;
    int status =0;

    //thread생성 후 status를 통해 data를 현재 받아도 되는지 아닌지 반환해줌.
    pthread_create(&p_thread[0], NULL, accept_client, (void*)&status);

    if(thr_id < 0){
        perror("thread create error");
    }

    init(201823785);

    while(1)
        {    
            sensordata = getSensorData();
            int lengthofData = strlen(dataToString(sensordata));
            write(fd, dataToString(sensordata),lengthofData);
            write(fd, "\n", 1);

            if(status){
                status = 0;
                //다시 thread 재생성으로 accept를 허용해줄 수 있게 함.
                pthread_create(&p_thread[0], NULL, accept_client, (void*)&status);

                if(thr_id < 0){
                    perror("thread create error");
                }

            }

            sleep(60);

        }
    return(0);
}

