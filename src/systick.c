#include <stdio.h>
#include <stdint.h>
#include "../libstm32f042k6/include/stm32f042k6.h"


static uint8_t f_switch = 0;


void SysTick_handler(void) {
  extern uint8_t g_is_adc_running;

  if(f_switch) {
    GPIO_OFF(GPIO_B, 3);
  } else {
    GPIO_ON(GPIO_B, 3);
  }
  f_switch ^= 1;

  if(! g_is_adc_running) {
    ADC->CR |= ADC_CR_ADSTART;
    g_is_adc_running = 1;
  }
}
