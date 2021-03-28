#ifndef _TEMP_USART_H
#define _TEMP_USART_H 1

#include <stdint.h>
#include "../libstm32f042k6/include/usart.h"

extern void usart_putchar(usart_t *usart, char c);

extern void usart_print(usart_t *usart, char *str);

extern void usart_println(usart_t *usart, char *str);

#endif
