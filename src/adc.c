//#define __DEBUG__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "../include/inline_asm.h"
#include "../include/usart.h"
#include "../libstm32f042k6/include/stm32f042k6.h"

/**
*/
extern char *uint16_to_hex(char *str, uint16_t value);


#define ADC_SIZE 4

uint16_t adc_values[ADC_SIZE];
static uint8_t _cursor = 0;

static float convert_voltage(uint16_t value);
static float calc_temperature(uint16_t value);
#ifdef __DEBUG__
static char *float_to_string(char *str, float value);
#endif

/**
*/
void ADC_COMP_handler(void) {
  extern uint8_t g_is_adc_running;
  uint32_t f = ADC->ISR;

  if((f & ADC_ISR_EOC) == ADC_ISR_EOC) {
    ADC->ISR = ADC_ISR_EOC;

    //uint32_t value = ADC->DR & 0x0fff;
    uint32_t value = ADC->DR & 0xfff;
    adc_values[_cursor++] = value;
  }


  if((f & ADC_ISR_EOSEQ) == ADC_ISR_EOSEQ) {
    ADC->CR |= ADC_CR_ADSTP;
    while((ADC->CR & ADC_CR_ADSTP) != 0) {
      nop();
    }
    ADC->ISR = ADC_ISR_EOSEQ;

    _cursor = 0;

    char buffer[8];
    int i = 0;
    for(i = 0; i < ADC_SIZE; ++i) {
      memset((void *)buffer, 0, sizeof(char) * 8);
      uint16_to_hex(buffer, *(adc_values + i));

      // USART1 へ出力を行う.
      usart_print(USART1, ".temp ");
      usart_putchar(USART1, '0' + i);
      usart_putchar(USART1, ' ');
      usart_println(USART1, buffer);
    }

#if 0
    float temp = calc_temperature(*adc_values);
    memset((void *)buffer, 0, sizeof(char) * 8);
    float_to_string(buffer, temp);
    usart_print(USART1, "#debug ");
    usart_println(USART1, buffer);
#endif

    // TODO: 取得した数値を元に処理を実行
    g_is_adc_running = 0;
  }

  if((f & ADC_ISR_OVR) == ADC_ISR_OVR) {
    ADC->ISR = ADC_ISR_OVR;
  }
}


static float convert_voltage(uint16_t value) {
  return (float)value * (3.3 / 4095.0);
}

static float calc_temperature(uint16_t value) {
  // サーミスタ(103JT-025) の B_25,85 定数(3435.0)
  const float B = 3435.0;
  // 抵抗 Ra 1[k ohm]
  const float ra = 1.0 * pow(10, 3);

  // ADC 計測値を電圧に変換
  float vb = convert_voltage(value);
  // 電源電圧 vcc = 3.3[V], 直列に接続した抵抗 Ra = 1.0[k ohm]
  // これらの値より, 抵抗 Rb を求める
  float rb = (ra * vb) / (3.3 - vb);


  // 取得した抵抗値を温度に変換
  return (B / logf((rb * exp(B / (25.0 + 273.15))) / 10000.0)) - 273.15;
}

#ifdef __DEBUG__
static char *float_to_string(char *str, float value) {
  uint16_t number = (uint16_t)value;
  uint8_t f_pt = (uint8_t)((value - number) * 10.0);

  int cnt = 0;
  uint16_t _number = number;
  while(_number > 0) {
    ++cnt;
    _number /= 10;
  }
  if(cnt > 0) {
    int i;
    for(i = 0; i < cnt; ++i) {
      *(str + (cnt - (i + 1))) = '0' + (number % 10);
      number /= 10;
    }
    *(str + cnt) = '.';
    *(str + (cnt + 1)) = '0' + f_pt;
  } else {
    *str = '0';
  }

  return str;
}
#endif
