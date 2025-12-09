long long powr(long long a, long long n) {
    if (n == 0) return 1;
    if (n % 2 == 1) return a * powr(a, n - 1);
    long long half = powr(a, n / 2);
    return half * half;
}