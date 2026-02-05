// main.c - TM4C123 UART Custom Implementation
// TM4C123GH6PM Datasheet: https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf

// ============ TM4C123 하드웨어 주소 정의 ============
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCUART_R (*((volatile unsigned long *)0x400FE618))

#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))

#define GPIO_PORTA_DIR_R (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_DEN_R (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AFSEL_R (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PCTL_R (*((volatile unsigned long *)0x4000452C))

#define UART0_DR_R (*((volatile unsigned long *)0x4000C000))
#define UART0_FR_R (*((volatile unsigned long *)0x4000C018))
#define UART0_IBRD_R (*((volatile unsigned long *)0x4000C024))
#define UART0_FBRD_R (*((volatile unsigned long *)0x4000C028))
#define UART0_LCRH_R (*((volatile unsigned long *)0x4000C02C))
#define UART0_CTL_R (*((volatile unsigned long *)0x4000C030))

#define LED_RED (1U << 1)
#define LED_GRN (1U << 2)
#define LED_BLU (1U << 3)

void delay(int ms)
{
    for (volatile int i = 0; i < ms; i++) ;
}

// ============ GPIO 초기화 ============
void gpio_init(void)
{
    // PORTF 클럭 활성화
    SYSCTL_RCGCGPIO_R |= 0x20;
    delay(100);
    
    // LED 핀 설정
    GPIO_PORTF_DIR_R |= (LED_RED | LED_GRN | LED_BLU);
    GPIO_PORTF_DEN_R |= (LED_RED | LED_GRN | LED_BLU);
}

// ============ UART 초기화 - 여기서부터 직접 구현 ============
void uart_init(void)
{
    // TODO: UART0 초기화 구현
    // 1. UART0 클럭 활성화
    // 2. GPIOA 클럭 활성화  
    // 3. PA0(RX), PA1(TX)을 UART alternate function으로 설정
    // 4. Baud rate 설정 (115200)
    // 5. UART0 활성화
}

void uart_putchar(char c)
{
    // TODO: 한 문자 전송 구현
}

void uart_puts(const char *str)
{
    // TODO: 문자열 전송 구현
}

int main(void)
{
    // GPIO 초기화
    gpio_init();
    
    // UART 초기화 (여러분이 구현)
    // uart_init();
    
    while (1)
    {
        // LED 깜빡이기
        GPIO_PORTF_DATA_R ^= LED_RED;
        delay(500000);
        
        // UART로 메시지 보내기 (구현 후 테스트)
        // uart_puts("Hello from TM4C123!\r\n");
    }

    return 0;
}
