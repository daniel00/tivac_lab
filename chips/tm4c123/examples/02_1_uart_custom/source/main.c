#include <stdint.h>
#include <stdarg.h>

/* ===============================
 *  Register definitions
 * =============================== */

/* Clock control */
#define SYSCTL_RCGCUART   (*((volatile uint32_t *)0x400FE618))
#define SYSCTL_RCGCGPIO   (*((volatile uint32_t *)0x400FE608))

/* GPIOA (UART0) */
#define GPIOA_AFSEL       (*((volatile uint32_t *)0x40004420))
#define GPIOA_DEN         (*((volatile uint32_t *)0x4000451C))
#define GPIOA_PCTL        (*((volatile uint32_t *)0x4000452C))

/* GPIOF (LED) */
#define GPIOF_DIR         (*((volatile uint32_t *)0x40025400))
#define GPIOF_DEN         (*((volatile uint32_t *)0x4002551C))
#define GPIOF_DATA        (*((volatile uint32_t *)0x400253FC))

/* UART0 */
#define UART0_DR          (*((volatile uint32_t *)0x4000C000))
#define UART0_FR          (*((volatile uint32_t *)0x4000C018))
#define UART0_IBRD        (*((volatile uint32_t *)0x4000C024))
#define UART0_FBRD        (*((volatile uint32_t *)0x4000C028))
#define UART0_LCRH        (*((volatile uint32_t *)0x4000C02C))
#define UART0_CTL         (*((volatile uint32_t *)0x4000C030))

#define UART_FR_TXFF      0x20

#define LED_RED           (1 << 1)   // PF1

/* ===============================
 *  Simple delay (busy wait)
 * =============================== */

static void delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = 1600;   // 16 MHz 기준 대략 1ms
        while (count--)
            ;
    }
}

/* ===============================
 *  UART init (115200, 8N1)
 * =============================== */

void uart_init(void)
{
    SYSCTL_RCGCUART |= (1 << 0);   // UART0 clock
    SYSCTL_RCGCGPIO |= (1 << 0);   // GPIOA clock

    UART0_CTL = 0;                 // disable UART

    // 16MHz / (16 * 115200) = 8.6805
    UART0_IBRD = 8;
    UART0_FBRD = 44;

    UART0_LCRH = (3 << 5);         // 8-bit, no parity, 1 stop
    UART0_CTL  = (1 << 0) | (1 << 8) | (1 << 9); // UART, TX, RX enable

    GPIOA_AFSEL |= (1 << 0) | (1 << 1); // PA0 RX, PA1 TX
    GPIOA_DEN   |= (1 << 0) | (1 << 1);

    GPIOA_PCTL &= ~0xFF;
    GPIOA_PCTL |= 0x11;
}

/* ===============================
 *  LED init (PF1)
 * =============================== */

void led_init(void)
{
    SYSCTL_RCGCGPIO |= (1 << 5);   // GPIOF clock

    GPIOF_DIR |= LED_RED;          // output
    GPIOF_DEN |= LED_RED;          // digital enable
}

/* ===============================
 *  Low-level UART output
 * =============================== */

static void uart_putc(char c)
{
    while (UART0_FR & UART_FR_TXFF)
        ;
    UART0_DR = c;
}

static void uart_puts(const char *s)
{
    while (*s)
    {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

/* ===============================
 *  Integer output
 * =============================== */

static void uart_put_int(int v)
{
    char buf[12];
    int i = 0;

    if (v == 0)
    {
        uart_putc('0');
        return;
    }

    if (v < 0)
    {
        uart_putc('-');
        v = -v;
    }

    while (v > 0)
    {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }

    while (i--)
        uart_putc(buf[i]);
}

/* ===============================
 *  Float output (output only)
 * =============================== */

/* ===============================
 *  uartprintf (minimal)
 * =============================== */

void uartprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    while (*fmt)
    {
        if (*fmt != '%')
        {
            uart_putc(*fmt++);
            continue;
        }

        fmt++; // skip %

        switch (*fmt)
        {
        case 's':
            uart_puts(va_arg(ap, char *));
            break;

        case 'd':
            uart_put_int(va_arg(ap, int));
            break;

        case 'c':
            uart_putc((char)va_arg(ap, int));
            break;


        case '%':
            uart_putc('%');
            break;

        default:
            uart_putc('?');
            break;
        }

        fmt++;
    }

    va_end(ap);
}

/* ===============================
 *  main
 * =============================== */

int main(void)
{
    uart_init();
    led_init();

    uartprintf("Tiva-C UART + LED test start\n");

    int count = 0;

    while (1)
    {
        GPIOF_DATA ^= LED_RED;   // LED toggle
        uartprintf("LED toggle %d\n\r", count++);
        delay_ms(500);
    }
}
