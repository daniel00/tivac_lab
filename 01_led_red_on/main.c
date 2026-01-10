// main.c
// 1. 하드웨어 주소 정의 (데이터시트의 보물지도)
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))

int main(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;

    // 딜레이 변수도 volatile로 선언하세요.
    volatile int delay;
    for (delay = 0; delay < 100; delay++)
        ;

    GPIO_PORTF_DIR_R |= 0x02;
    GPIO_PORTF_DEN_R |= 0x02;

    while (1)
    {
        // 토글
        GPIO_PORTF_DATA_R ^= 0x02;

        // 컴파일러가 삭제하지 못하도록 volatile 사용
        // 16MHz 클럭에서 20,000은 너무 순식간일 수 있으니 200,000 정도로
        // 늘려보세요.
        //yoo 컴파일의 최적화 옵션이 디폴트로 -O0 이므로 volatile을 붙이지 않아도 최적화를 수행하지 않는다.
        //따라서 아래처럼 사용해도 된다.
        //1000000: 약 1초이다.
        //
        for (int i = 0; i < 1000000; i++) ;
    }
}
