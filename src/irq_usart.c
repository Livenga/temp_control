#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../include/command.h"
#include "../include/inline_asm.h"
#include "../include/util.h"
#include "../include/usart.h"
#include "../libstm32f042k6/include/stm32f042k6.h"


#define is_newline(chr) \
  (chr == 0x0d || chr == 0x0a)
#define DEC2HEX(dec) \
  (dec & 0x0f) >= 0x0A\
  ? 'A' + ((dec & 0x0f) - 0x0A)\
  : '0' + (dec & 0x0f)

#define BUFFER_SIZE (32)
static char _buffer[BUFFER_SIZE];
static uint8_t _buffer_cursor = 0;


static void _usart_execute_command();

/**
 * デバッグフラグ切り替え
 */
static void _switch_debug_mode(char *args);
static void _switch_relay_flag(char *args);
static void _switch_pwm_flag(char *args);

/**
*/
void USART1_handler(void) {
  if((USART1->ISR & USART_ISR_RXNE) != 0) {
    // データ受信
    uint8_t rcv = USART1->RDR;
    USART1->RQR = USART_RQR_RXFRQ;

#ifdef __DEBUG__
    usart_print(USART1, "0x");
    usart_putchar(USART1, DEC2HEX(rcv >> 4));
    usart_putchar(USART1, DEC2HEX(rcv >> 0));
    usart_putchar(USART1, '\r');
    usart_putchar(USART1, '\n');
#endif

    if(is_newline(rcv)) {
      // Command execute
      *(_buffer + _buffer_cursor) = '\0';

      _usart_execute_command();
    } else {
      switch(rcv) {
        case 0x7f: // Backspace
          if(_buffer_cursor > 0) {
            *(_buffer + _buffer_cursor--) = '\0';
          }
          break;

        default:
          *(_buffer + _buffer_cursor++) = rcv;
          if(_buffer_cursor == BUFFER_SIZE) {
            // Command execute
            _usart_execute_command();
          }
          break;
      }
    }
  }
}


/**
*/
static void _usart_execute_command() {
  extern struct command_t g_command;

  adjust_string(_buffer, ' ');
#ifdef __DEBUG__
  usart_println(USART1, _buffer);
#endif

  char *p = NULL;
  p = strchr(_buffer, ' ');
  if(p != NULL) {
    *p++ = '\0';
  }

  if(strcmp(_buffer, "debug") == 0) {
    _switch_debug_mode(p);
  } else if(strcmp(_buffer, "relay") == 0) {
#ifdef __DEBUG__
    usart_println(USART1, "execute relay command");
#endif
    _switch_relay_flag(p);
  } else if(strcmp(_buffer, "pwm") == 0) {
#ifdef __DEBUG__
    usart_println(USART1, "execute pwm command");
#endif
    _switch_pwm_flag(p);
  }

  _buffer_cursor = 0;
  memset((void *)_buffer, '\0', BUFFER_SIZE);
}


/**
 * デバッグフラグ切り替え
 */
static void _switch_debug_mode(char *args) {
  extern uint8_t g_is_debug;
  if(args == NULL) {
    return;
  }

  char *p = strchr(args, ' ');
  if(p != NULL) {
    *p = '\0';
  }

  if(strcmp(args, "on") == 0) {
    g_is_debug = 1;
  } else if(strcmp(args, "off") == 0) {
    g_is_debug = 0;
  }
}

/**
 */
static void _switch_relay_flag(char *args) {
  extern uint8_t g_is_debug;
  if(! g_is_debug || args == NULL) {
    return;
  }
  char *p = strchr(args, ' ');
  if(p != NULL) {
    *p = '\0';
  }

  if(strcmp(args, "on") == 0) {
    GPIO_ON(GPIO_A, 9);
  } else if(strcmp(args, "off") == 0) {
    GPIO_OFF(GPIO_A, 9);
  }
}

/**
 */
static void _switch_pwm_flag(char *args) {
  extern uint8_t g_is_debug;
  if(! g_is_debug || args == NULL) {
    return;
  }
  char *p = strchr(args, ' ');
  if(p != NULL) {
    *p = '\0';
  }

  if(strcmp(args, "on") == 0) {
    GPIO_ON(GPIO_A, 8);
  } else if(strcmp(args, "off") == 0) {
    GPIO_OFF(GPIO_A, 8);
  }
}
