#include <ch375.h>
#include <uart.h>
#include <xc.h>


void __interrupt() isr(void)
{
    if (INTCONbits.INT0IF) {
        usb_handler();
        INTCONbits.INT0IF = 0;
    }
}


void main(void)
{
    uart_init();
    printf("hello\n");
    ch375_init();
    while (1)
        ;
}
