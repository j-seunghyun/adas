/**
* 센서의 측정값을 담는 구조체
*
* Member : char* stamp - 타임스탬프
* char* value - 측정값
*/
typedef struct {  
    char* stamp;
    char* value;
} DATA;
/**
* Data를 받기 전 본인의 학번으로 프로그램을 초기화.
*
* Param : int id - 학번
* return: 유효한 경우 1
*/
int init(int id);
/**
* 센서 데이터를 받음
*
* return: 현재 센서의 측정값
* throw : init()이 먼저 호출되지 않은 경우.
*/
DATA* getSensorData();
/**
* DATA 구조체를 문자열로 반환하는 유틸.
*
* Param : DATA* - 문자열로 바꿀 데이터 대상.
* return: stamp 및 value의 문자열.
*/
char* dataToString(DATA* data);
