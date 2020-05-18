/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * minimal sprintf implementation
 */

#include "sys.h"

void putchar(char ch){
  char buffer[2];
  buffer[0] = ch, buffer[1] = 0;
  call_sys_write(buffer);
}

unsigned int vsprintf(char *dst, char *fmt, __builtin_va_list args) {
  long int arg;
  int len, sign, i;
  char *p, *orig = dst, tmpstr[19];

  // failsafes
  if (fmt == (void *)0) {
    return 0;
  }

  // main loop
  arg = 0;
  while (*fmt) {
    // argument access
    if (*fmt == '%') {
      fmt++;
      // literal %
      if (*fmt == '%') {
        goto put;
      }
      len = 0;
      // size modifier
      while (*fmt >= '0' && *fmt <= '9') {
        len *= 10;
        len += *fmt - '0';
        fmt++;
      }
      // skip long modifier
      if (*fmt == 'l') {
        fmt++;
      }
      // character
      if (*fmt == 'c') {
        arg = __builtin_va_arg(args, int);
        if (dst)
          *dst++ = (char)arg;
        else
          putchar((char)arg);
        fmt++;
        continue;
      } else
          // decimal number
          if (*fmt == 'd') {
        arg = __builtin_va_arg(args, int);
        // check input
        sign = 0;
        if ((int)arg < 0) {
          arg *= -1;
          sign++;
        }
        if (arg > 99999999999999999L) {
          arg = 99999999999999999L;
        }
        // convert to string
        i = 18;
        tmpstr[i] = 0;
        do {
          tmpstr[--i] = '0' + (arg % 10);
          arg /= 10;
        } while (arg != 0 && i > 0);
        if (sign) {
          tmpstr[--i] = '-';
        }
        // padding, only space
        if (len > 0 && len < 18) {
          while (i > 18 - len) {
            tmpstr[--i] = ' ';
          }
        }
        p = &tmpstr[i];
        goto copystring;
      } else
          // hex number
          if (*fmt == 'x') {
        arg = __builtin_va_arg(args, long int);
        // convert to string
        i = 16;
        tmpstr[i] = 0;
        do {
          char n = arg & 0xf;
          // 0-9 => '0'-'9', 10-15 => 'A'-'F'
          tmpstr[--i] = n + (n > 9 ? 0x37 : 0x30);
          arg >>= 4;
        } while (arg != 0 && i > 0);
        // padding, only leading zeros
        if (len > 0 && len <= 16) {
          while (i > 16 - len) {
            tmpstr[--i] = '0';
          }
        }
        p = &tmpstr[i];
        goto copystring;
      } else
          // string
          if (*fmt == 's') {
        p = __builtin_va_arg(args, char *);
      copystring:
        if (p == (void *)0) {
          p = "(null)";
        }
        while (*p) {
          if (dst)
            *dst++ = *p++;
          else
            putchar(*p++);
        }
      }
    } else {
    put:
      if (dst)
        *dst++ = *fmt;
      else
        putchar(*fmt);
    }
    fmt++;
  }
  if (dst)
    *dst = 0;
  // number of bytes written
  return dst - orig;
}

/**
 * Variable length arguments
 */
unsigned int sprintf(char *dst, char *fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  return vsprintf(dst, fmt, args);
}

/**
 * Display a string
 */
void printf(char *fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  // we don't have memory allocation yet, so we
  // simply place our string after our code
  // use sprintf to format our string
  vsprintf(0, fmt, args);
}