#pragma once
#include <stdarg.h>
#ifdef __XC8
#include <xc.h>
#else
#include <stdint.h>
#include "dfp/xc8/pic/include/proc/pic18f4520.h"
#define __interrupt(priority)
#define bit unsigned char
#define bool unsigned char
#define NULL 0
#endif

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = OFF
#define _XTAL_FREQ 40000000ULL
