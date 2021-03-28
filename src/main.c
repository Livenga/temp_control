//#define __DEBUG__
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/inline_asm.h"
#include "../include/usart.h"
#include "../libstm32f042k6/include/stm32f042k6.h"

uint8_t g_is_adc_running = 0;

static void io_init(void);

static void adc_init(void);

static void usart_init(void);


/**
 */
int main(void) {
  io_init();

  usart_init();

  adc_init();



  // Systick(動作確認用)
  SYSTICK->CSR = STK_CSR_CLKSOURCE | STK_CSR_TICKINT | STK_CSR_ENABLE;
#ifdef __DEBUG__
  SYSTICK->RVR = 0x003d08ff; // 0.5[s]
#else
  SYSTICK->RVR = 0x007a11ff; // 1.0[s]
#endif


  Enable_NVIC(ADC_COMP_IRQn);
  while(1) {
    nop();
  }

  return 0;
}


/**
 */
static void io_init(void) {
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  RCC->CR |= RCC_CR_HSION;
  while((RCC->CR & RCC_CR_HSIRDY) == 0) {
    nop();
  }

  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
  while((RCC->CFGR & ~RCC_CFGR_SWS_HSI) != RCC_CFGR_SWS_HSI) {
    nop();
  }


  RCC->AHBENR |= RCC_AHBENR_IOPAEN
    | RCC_AHBENR_IOPBEN;

  // A/D Converter, Usart1 有効化
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN | RCC_APB2ENR_USART1EN;
  RCC->CFGR3 = (RCC->CFGR3 & ~RCC_CFGR3_USART1SW) | RCC_CFGR3_USART1SW_SYSCLK;

  RCC->CFGR3 = (RCC->CFGR3 & ~RCC_CFGR3_USART1SW) | RCC_CFGR3_USART1SW_HSI;


  // PA0, PA1, ADC
  GPIO_SET_MODE(GPIO_A, 0, GPIO_MODER_ANALOG);
  GPIO_SET_MODE(GPIO_A, 1, GPIO_MODER_ANALOG);

  // USART Tx
  GPIO_SET_MODE(GPIO_A, 9, GPIO_MODER_ALTERNATE_FUNCTION);
  // USART Rx
  GPIO_SET_MODE(GPIO_A, 10, GPIO_MODER_ALTERNATE_FUNCTION);

  // LED 点滅用 GPIO B 3
  GPIO_SET_MODE(GPIO_B, 3, GPIO_MODER_OUTPUT);

  //
  GPIO_A->AFRH = (GPIO_A->AFRH & ~(GPIO_AFRH_AFSEL9 | GPIO_AFRH_AFSEL10))
    | (GPIO_AFRH_AFSEL_AF1 << 4)
    | (GPIO_AFRH_AFSEL_AF1 << 8);
}

/**
*/
static void adc_init(void) {
  ADC->CR = ADC_CR_ADCAL;
  while((ADC->CR & ADC_CR_ADCAL) != 0) {
    nop();
  }

  if((ADC->ISR & ADC_ISR_ADRDY) != 0) {
    ADC->ISR |= ADC_ISR_ADRDY;
  }
  ADC->CR |= ADC_CR_ADEN;
  while((ADC->ISR & ADC_ISR_ADRDY) == 0) {
    nop();
  }

  //ADC->CFGR1 = ADC_CFGR1_CONT | ADC_CFGR1_SCANDIR;
  ADC->CFGR1 = ADC_CFGR1_CONT;
  ADC->CFGR2 = 0;

  // sampling time selection
  //ADC->SMPR  = 0b011; // 28.5 ADC clock
  ADC->SMPR  = 0b111;

  ADC->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL17;
  //ADC->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1;
  ADC->IER |= ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE;
  ADC->CCR |= ADC_CCR_VREFEN;
}

/**
*/
static void usart_init(void) {
#if 1
  USART1->CR1 =
    USART_CR1_RXNEIE | USART_CR1_TCIE
    | USART_CR1_TE | USART_CR1_RE;
#else
  USART1->CR1 = USART_CR1_TE | USART_CR1_RE;
#endif

  USART1->BRR = 0x0045;
  //USART1->BRR = 0x008b;
  USART1->CR1 |= USART_CR1_UE;
}
