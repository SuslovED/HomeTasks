program {
    bool a, b, c;
    int x, y;
    a = true;
    b = false;
    c = a and not b or false;
    write(a, b, c);
    x = 10;
    y = 20;
    if (x < y and not b) write(x);
}
