/*
Задача 1 (переформулируем чётко).
Требуется написать свою функцию over_fprintf(int fd, const char *fmt, ...) с поддержкой как минимум:

%o — восьмеричное без префикса
%x%X — шестнадцатеричное
%p — указатель как 0x...
%aa %A — double в шестнадцатеричной форме (0x1.8p+10 и т.д.)

ИДЕЯ РЕШЕНИЯ

Проходим по строке формата посимвольно.
Обычные символы выводим сразу через write(fd, ...)
При встрече % разбираем спецификатор.
Все целые числа переводим в строку одной универсальной функцией to_base с реверсом буфера.
Для %a/%A используем битовые операции над представлением double (IEEE-754):
вытаскиваем знак, экспоненту и мантиссу
нормализуем к виду 1.m × 2^exp
выводим целую часть (1 или 0), точку и 13 шестн. цифр мантиссы
выводим p±exp
*/


#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>

static void put_char(int fd, char c) { write(fd, &c, 1); }
static void put_str(int fd, const char *s) { while (*s) put_char(fd, *s++); }

static void put_rev(int fd, char *buf, int len) {
    while (len--) put_char(fd, buf[len]);
}

static int to_base(uintmax_t n, char *buf, int base, int up) {
    const char *d = up ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;
    do { buf[i++] = d[n % base]; } while (n /= base);
    return i;
}

static void print_hex_double(int fd, double x, int up) {
    if (x < 0) { put_char(fd, '-'); x = -x; }
    if (__builtin_isinf(x)) { put_str(fd, up ? "INF" : "inf"); return; }
    if (__builtin_isnan(x)) { put_str(fd, up ? "NAN" : "nan"); return; }
    uint64_t bits;
    __builtin_memcpy(&bits, &x, 8);
    int sign = bits >> 63;
    int exp  = (bits >> 52) & 0x7FF;
    uint64_t mant = bits & 0xFFFFFFFFFFFFFULL;
    put_str(fd, up ? "0X" : "0x");
    if (exp == 0 && mant == 0) { put_str(fd, "0p+0"); return; }
    if (exp == 0) exp = 1; else mant |= 1ULL << 52;  // denormal → normal
    exp -= 1023 + 52;

    put_char(fd, '1');
    put_char(fd, '.');

    char digits[20];
    int pos = 0;
    for (int i = 0; i < 52; i += 4) {
        int dig = (mant >> (48 - i)) & 0xF;
        digits[pos++] = (dig < 10 ? '0' + dig : (up ? 'A' : 'a') + dig - 10);
    }
    put_rev(fd, digits, pos);

    put_char(fd, up ? 'P' : 'p');
    if (exp >= 0) put_char(fd, '+'); else { put_char(fd, '-'); exp = -exp; }
    char ebuf[10];
    int elen = to_base(exp, ebuf, 10, 0);
    put_rev(fd, ebuf, elen);
}

int over_fprintf(int fd, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') { put_char(fd, *fmt++); continue; }
        fmt++;
        int up = (*fmt == 'X' || *fmt == 'A' || *fmt == 'P');
        char buf[66];
        int len;
        switch (*fmt) {
            case '%': put_char(fd, '%'); break;
            case 'd': {
                int v = va_arg(ap, int);
                if (v < 0) { put_char(fd, '-'); v = -v; }
                len = to_base(v, buf, 10, 0);
                put_rev(fd, buf, len);
                break;
            }
            case 'u': len = to_base(va_arg(ap, unsigned), buf, 10, 0); put_rev(fd, buf, len); break;
            case 'o': len = to_base(va_arg(ap, unsigned), buf, 8, 0);  put_rev(fd, buf, len); break;
            case 'x': case 'X':
                len = to_base(va_arg(ap, unsigned), buf, 16, up);
                put_rev(fd, buf, len);
                break;
            case 'p':
                put_str(fd, "0x");
                len = to_base((uintptr_t)va_arg(ap, void*), buf, 16, 0);
                put_rev(fd, buf, len);
                break;
            case 'a': case 'A':
                print_hex_double(fd, va_arg(ap, double), up);
                break;
            case 's': {
                char *s = va_arg(ap, char*);
                put_str(fd, s ? s : "(null)");
                break;
            }
            case 'c': put_char(fd, va_arg(ap, int)); break;
        }
        fmt++;
    }
    va_end(ap);
    return 0;
}