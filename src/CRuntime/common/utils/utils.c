#include "utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>


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
  while(str[l++]);
  return l;
}

size_t cr_itoa(int value, char *sp, size_t radix)
{
    char tmp[16];
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

int cr_vsnprintf(char* buf, size_t size, const char* fmt, ...)
{
  va_list arg;
  size_t written=0;
  size_t temp_writ=0;
  bool next_fmt = false;
  int d;
  size_t u;
  char c, in_c;
  char* s = buf, *ts;

  va_start(arg, fmt);

  while (*fmt)
  {
    if(written > size) return written;

    in_c = *fmt++;
    switch (in_c)
    {
      case '%':
        next_fmt = true;
        break;
      case 'd':
        if(next_fmt)
        {
          d = va_arg(arg, int);
          temp_writ = cr_itoa(d, s ,10);
          s+=temp_writ;
          written+=temp_writ;
          next_fmt=false;
          break;
        }
        goto cs_vsnprintf_defaut;
      case 'u':
        if(next_fmt)
        {
          u = va_arg(arg, int);
          temp_writ = cr_itoa(u, s ,10);
          s+=temp_writ;
          written+=temp_writ;
          next_fmt=false;
          break;
        }
        goto cs_vsnprintf_defaut;

      case 'x':
        if(next_fmt)
        {
          u = va_arg(arg, int);
          temp_writ = cr_itoa(u, s ,16);
          s+=temp_writ;
          written+=temp_writ;
          next_fmt=false;
          break;
        }
        goto cs_vsnprintf_defaut;
      case 'c':
        if(next_fmt)
        {
          c = (char) va_arg(arg, int);
          *(s++) = c;
          written++;
          next_fmt = false;
          break;
        }
      case 's':
        if(next_fmt)
        {
          ts = va_arg(arg, char*);
          while(*ts)
          {
            written++;
            *(s++) = *(ts++);
          }
          next_fmt = false;
          break;
        }
        goto cs_vsnprintf_defaut;
      default:
cs_vsnprintf_defaut:
        *(s++) = in_c;
        written++;
    
    }
  }


  return written;
}
