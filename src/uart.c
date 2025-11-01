#include <uart.h>

void uart_init(void)
{
    unsigned long baud = 9600;
    unsigned long spbrg;

    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 1;

    spbrg = (_XTAL_FREQ / (4UL * baud)) - 1;

    SPBRG = spbrg & 0xFF;
    SPBRGH = (spbrg >> 8) & 0xFF;

    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
}

inline void uart_putchar(char c)
{
    while (!TXSTAbits.TRMT)
        ;
    TXREG = c;
}

inline char uart_getchar(void)
{
    while (!PIR1bits.RCIF)
        ;
    return RCREG;
}
void putchar(char c)
{
    if (c == '\n')
        uart_putchar('\r');
    uart_putchar(c);
}

static void print_uint(unsigned int x)
{
    char tmp[10];
    int i = 0;
    tmp[0] = '0';
    while (x) {
        tmp[i++] = x % 10 + '0';
        x /= 10;
    }
    i--;
    while (i >= 0)
        putchar(tmp[i--]);
}

void printf(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            putchar(*fmt);
            continue;
        }
        fmt++;
        switch (*fmt) {
        case '\0':
            goto DONE;
        case 's': {
            const char *s = va_arg(vargs, const char *);
            while (*s)
                putchar(*s++);
            break;
        }
        case 'c':
            putchar(*fmt);
            break;
        case 'd': {
            int x = va_arg(vargs, int);
            if (x < 0) {
                putchar('-');
                x = -x;
            }
            print_uint((unsigned int) x);
            break;
        }
        case 'u': {
            unsigned int y = va_arg(vargs, unsigned int);
            print_uint(y);
            break;
        }
        case 'x': {
            unsigned int z = va_arg(vargs, unsigned int);
            for (int i = 7; i >= 0; i--) {
                putchar("0123456789abcdef"[(z >> (i << 2)) & 0xf]);
            }
            break;
        }
        case 'l': {
            fmt++;
            unsigned long z = va_arg(vargs, unsigned long);
            for (int i = 15; i >= 0; i--) {
                putchar("0123456789abcdef"[(z >> (i << 2)) & 0xf]);
            }
            break;
        }
        }
    }
DONE:
    va_end(vargs);
}
