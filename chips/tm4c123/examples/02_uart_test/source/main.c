// main.c - TM4C123 UART + LED Test
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

// ============ UART 함수 ============
// UART0 초기화 (115200 baud rate)
void uart_init(void)
{
    // UART0 클럭 활성화
    SYSCTL_RCGCUART_R |= 0x01;
    // GPIOA 클럭 활성화
    SYSCTL_RCGCGPIO_R |= 0x01;
    
    delay(10);
    
    // PA0, PA1을 UART로 설정
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTA_DEN_R |= 0x03;
    
    // UART0 비활성화
    UART0_CTL_R = 0;
    
    // 보드레이트 설정 (16MHz 클록, 115200 baud)
    // BRD = 16,000,000 / (16 * 115200) = 8.68
    UART0_IBRD_R = 8;
    UART0_FBRD_R = 44;
    
    // 8비트, 1 stop bit, no parity, no FIFOs
    UART0_LCRH_R = 0x60;
    
    // UART0 활성화
    UART0_CTL_R = 0x301;
}

// UART0로 한 문자 전송
void uart_putchar(char c)
{
    // TX FIFO가 비워질 때까지 대기
    while (UART0_FR_R & 0x20);
    UART0_DR_R = c;
}

// UART0로 문자열 전송
void uart_puts(const char *str)
{
    while (*str) {
        uart_putchar(*str++);
    }
}

// ============ 공통 printf 함수 ============

// 간단한 UART printf 구현
void uart_printf(const char *fmt, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    
    char buf[256];
    int len = 0;
    
    for (const char *p = fmt; *p && len < 255; p++) {
        if (*p == '%' && *(p+1)) {
            p++;
            switch (*p) {
                case 'd': {
                    int val = __builtin_va_arg(ap, int);
                    int divisor = 1000000000;
                    int started = 0;
                    for (int i = 0; i < 10; i++) {
                        int digit = (val / divisor) % 10;
                        if (digit != 0 || started || divisor == 1) {
                            buf[len++] = '0' + digit;
                            started = 1;
                        }
                        divisor /= 10;
                    }
                    if (!started) buf[len++] = '0';
                    break;
                }
                case 'x': {
                    unsigned int val = __builtin_va_arg(ap, unsigned int);
                    for (int i = 7; i >= 0; i--) {
                        int nibble = (val >> (i * 4)) & 0xF;
                        buf[len++] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
                    }
                    break;
                }
                case 's': {
                    const char *str = __builtin_va_arg(ap, const char *);
                    while (*str && len < 255) buf[len++] = *str++;
                    break;
                }
                case 'c':
                    buf[len++] = __builtin_va_arg(ap, int);
                    break;
                case '%':
                    buf[len++] = '%';
                    break;
            }
        } else {
            buf[len++] = *p;
        }
    }
    
    __builtin_va_end(ap);
    
    buf[len] = '\0';
    uart_puts(buf);
}

int main(void)
{
    int delaycnt = 500000;
    int counter = 0;

    // UART 초기화
    uart_init();
    uart_printf("=== TM4C123 UART+LED Test ===\r\n");
    uart_printf("UART Initialized at 115200 baud\r\n");
    
    // GPIO 초기화
    gpio_init();
    uart_printf("GPIO Initialized\r\n");

    while (1)
    {
        uart_puts("test string\r\n");

        // RED LED 토글
        GPIO_PORTF_DATA_R ^= LED_RED;
        uart_printf("RED LED toggle count: %d\r\n", counter++);
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_RED;

        // GREEN LED 토글
        GPIO_PORTF_DATA_R ^= LED_GRN;
        uart_printf("GREEN LED toggle\r\n");
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_GRN;

        // BLUE LED 토글
        GPIO_PORTF_DATA_R ^= LED_BLU;
        uart_printf("BLUE LED toggle\r\n");
        delay(delaycnt);
        GPIO_PORTF_DATA_R ^= LED_BLU;
    }

    return 0;
}
