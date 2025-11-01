#include "xc.inc"

GLOBAL _ch375_gpio_init
PSECT mytext,local,class=CODE, reloc=2
 

; GPIOA0: A0, 1 is cmd, 0 is data
; GPIOA1: WR
; GPIOA2: RD
; GPIOD: D0-D7 


_ch375_gpio_init:
    BSF TRISB, 0
    SETF TRISD
    BCF TRISA, 0
    BCF TRISA, 1
    BCF TRISA, 2
    BSF LATA, 1
    BSF LATA, 2
    RETURN
