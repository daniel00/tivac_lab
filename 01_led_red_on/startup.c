// startup.c
#define STACK_TOP 0x20008000  // TM4C123의 RAM 끝 지점 (32KB)

// 함수 선언
void Reset_Handler(void);
extern int main(void);

// 벡터 테이블 (Vector Table)
// 칩이 켜지자마자 0번지부터 차례대로 읽는 '약속된 표'입니다.
__attribute__ ((section(".vectors")))
void (* const vector_table[])(void) = {
    (void (*)(void))STACK_TOP,  // 0번지: 스택의 시작 주소
    Reset_Handler               // 4번지: 리셋 시 실행할 함수 주소
};

// 전원이 켜지면 실행되는 진짜 첫 번째 함수
void Reset_Handler(void) {
    // 여기에 원래는 변수 초기화 코드가 들어가야 하지만, 
    // 지금은 가장 단순하게 바로 main으로 점프합니다!
    main();
    
    while(1); // main이 끝나버리면 여기서 무한 대기
}
