program {
    int i, j;
    i = 1;
    while (i <= 3) {
        j = 1;
        while (j <= 3) {
            if (i == j) {
                j = j + 1;
                continue;
            }
            write(i, j);
            if (j == 2) break;
            j = j + 1;
        }
        i = i + 1;
    }
}
