#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../include/inline_asm.h"
#include "../libstm32f042k6/include/stm32f042k6.h"


void usart_putchar(usart_t *usart, char c) {
  usart->TDR = c;
  while((usart->ISR & USART_ISR_TXE) == 0) {
    nop();
  }

  usart->ICR = USART_ICR_TCCF;
}

void usart_print(usart_t *usart, char *str) {
  while(*str) {
    usart_putchar(usart, *str++);
  }
}

void usart_println(usart_t *usart, char *str) {
  usart_print(usart, str);
  usart_print(usart, "\r\n");
}
