/*
 * File:   interrupt.c
 * Author: ttama
 *
 * Created on 2020/02/08, 14:19
 */


#include "system.h"

void __interrupt () my_isr_routine (void)
{
    _UART_INTERRUPT_TASK();
    
}
