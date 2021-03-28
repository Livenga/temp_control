#include <stdio.h>
#include <stdint.h>

#define DEC2CHAR(value) \
  ((value > 9) ? 'A' + (value - 10) : '0' + value);

char *uint16_to_hex(char *str, uint16_t value) {
  if(str == NULL) {
    return NULL;
  }

  int i;
  for(i = 0; i < 4; ++i) {
    uint8_t v = (uint8_t)((value >> ((3 - i) * 4)) & 0x0f);
    *(str + i) = DEC2CHAR(v);
  }

  return str;
}
