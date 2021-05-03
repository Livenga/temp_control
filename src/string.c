#include <stdio.h>
#include <stdint.h>
#include <string.h>

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


/**
 */
char *adjust_string(
    char *src,
    char chr) {
  if(strlen(src) == 0) {
    return src;
  }

  int i = 0;
  // 末尾の削除
  for(i = strlen(src) - 1; *(src + i) == chr; --i) {
    *(src + i) = '\0';
  }

  char *p = NULL;
  for(p = strchr(src, chr); p != NULL; p = strchr(p, chr)) {
    ++p;
    i = 0;
    while(*(p + i) == chr) {
      ++i;
    }

    if(i > 0) {
      int size = strlen(p + i);
      memcpy(p, p + i, size);
      *(p + size) = '\0';
    }
  }

  return src;
}
