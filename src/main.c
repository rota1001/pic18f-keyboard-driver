#include <ch375.h>
#include <uart.h>
#include <xc.h>

extern void init(void);


void main(void)
{
    uart_init();
    while (1)
        ;
}