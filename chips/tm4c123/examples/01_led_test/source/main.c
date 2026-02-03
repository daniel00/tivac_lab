// main.c - TM4C123 LED Blink Test
// TM4C123GH6PM Datasheet: https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf

// ============ TM4C123 하드웨어 주소 정의 ============
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))

#define LED_RED (1U << 1)
#define LED_GRN (1U << 2)
#define LED_BLU (1U << 3)

void delay(int ms)
{
    for (volatile int i = 0; i < ms; i++) ;
}

// ============ GPIO 초기화 함수 ============
void gpio_init(void)
{
    // PORTF 클럭 활성화
    SYSCTL_RCGCGPIO_R |= 0x20;

    // 전원이 퍼져나가는 시간 딜레이
    delay(100);

    // LED 핀 설정
    GPIO_PORTF_DIR_R |= (LED_RED | LED_GRN | LED_BLU);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_GRN | LED_BLU);
}



int main(void)
{
    int delaycnt = 100000;

    // GPIO 초기화
    gpio_init();

    while (1)
    {
        // RED LED 토글
        GPIO_PORTF_DATA_R ^= LED_RED;
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_RED;

        // GREEN LED 토글
        GPIO_PORTF_DATA_R ^= LED_GRN;
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_GRN;

        // BLUE LED 토글
        GPIO_PORTF_DATA_R ^= LED_BLU;
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_BLU;
    }

    return 0;
}

// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c main.c -o main.o
// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -c startup.c -o startup.o
// arm-none-eabi-ld -T tm4c123.ld startup.o main.o -o final.elf
// arm-none-eabi-objcopy -O binary final.elf final.bin
// sudo lm4flash final.bin

