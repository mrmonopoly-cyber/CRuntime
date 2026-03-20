#include "utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

void *cr_memset(void *s, int c, size_t n)
{
  unsigned char* p = (unsigned char*) s;
  while(n--)
  {
    *p++ = c;
  }

  return s;
}

size_t cr_strlen(const char* str)
{
  size_t l=0;
  while(str[l]) l++;
  return l;
}

size_t cr_itoa(int value, char *sp, size_t radix)
{
    char tmp[32];
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);    
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign) 
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    return len;
}

size_t cr_vsnprintf_arg(char *restrict s, size_t n, const char* fmt, va_list arg)
{
  size_t written =0, len;
  char in_c;
  bool command = false;
  int d;
  unsigned int u;
  char c;
  char *sp;

  while (*fmt)
  {
    in_c = *fmt++;
    switch (in_c)
    {
      case '%':
        command = true;
        break;
      case 'c':
        if(command && written + sizeof(char) < n)
        {
          c = (char) va_arg(arg, int);
          *(s++) = c;
          written++;
          command = false;
          break;
        }
        goto vsnprintf_default;
      case 'd':
        if(command && written + sizeof(int) < n)
        {
          d = va_arg(arg, int);
          len= cr_itoa(d, s, 10);
          written+=len;
          s+=len;
          command = false;
          break;
        }
        goto vsnprintf_default;
      case 'u':
        if(command && written + sizeof(unsigned int) < n)
        {
          u = va_arg(arg, unsigned int);
          len= cr_itoa(u, s, 10);
          written+=len;
          s+=len;
          command = false;
          break;
        }
        goto vsnprintf_default;
      case 'x':
        if(command && written + sizeof(int) < n)
        {
          d = va_arg(arg, int);
          len= cr_itoa(d, s, 16);
          written+=len;
          s+=len;
          command = false;
          break;
        }
        goto vsnprintf_default;
      case 's':
        if(command)
        {
          sp = va_arg(arg, char*);
          while(written + 1 < n && *sp)
          {
            *(s++) = *(sp++);
            written++;
          }
          command = false;
          break;
        }
        goto vsnprintf_default;
      default:
vsnprintf_default:
        if(written + sizeof(char) < n)
        {
          *(s++) = in_c;
          written++;
        }
        break;
    }
  }

  return written;
}

size_t cr_vsnprintf(char* buf, size_t size, const char* fmt, ...)
{
  va_list arg;
  size_t written=0;

  va_start(arg, fmt);
  written = cr_vsnprintf_arg(buf, size, fmt, arg);
  va_end(arg);

  return written;
}
