#include <stdio.h>
#include <stdint.h>


/**
 */
void init_mem(void) {
  extern uint8_t *__bss_start__, *__bss_end__,
         *__text_start__,
         *__data_start__, *__data_end__;

  uint8_t *src = NULL, *dst = NULL;
  src = (uint8_t *)&__text_start__;
  dst = (uint8_t *)&__data_start__;
  for(;
      dst < (uint8_t *)&__data_end__;
      ++dst, ++src) {
    *dst = *src;
  }

  for(dst = (uint8_t *)&__bss_start__;
      dst < (uint8_t *)&__bss_end__;
      ++dst) {
    *dst= '\0';
  }
}
