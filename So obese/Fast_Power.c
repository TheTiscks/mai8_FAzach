long long binpow_iter(long long a, long long n) {
    long long res = 1;
    while (n > 0) {
        if (n & 1)          // если младший бит = 1, чётн
            res *= a;
        a *= a;             // переходим к следующему биту
        n >>= 1;    // отсекаем ласт бит
    }
    return res;
}