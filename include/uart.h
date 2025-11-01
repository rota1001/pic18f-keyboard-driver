#pragma once
#include <defs.h>


void uart_init(void);

inline void uart_putchar(char c);

inline char uart_getchar(void);

void printf(const char *fmt, ...);
