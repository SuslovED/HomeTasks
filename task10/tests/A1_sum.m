program {
    int n, sum, i;
    read(n);
    sum = 0;
    i = 1;
    while (i <= n) {
        sum = sum + i;
        i = i + 1;
    }
    write(sum);
}
