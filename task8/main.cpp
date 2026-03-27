#include "matrix.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

void runTest(int testNum, const char* desc, bool condition) {
    if (condition) {
        cerr << "Test " << testNum << " (" << desc << "): PASSED" << endl;
    } else {
        cerr << "Test " << testNum << " (" << desc << "): FAILED" << endl;
    }
}

int main(int argc, char* argv[]) {
    bool runTests = (argc == 1) || (argc == 2 && strcmp(argv[1], "test") == 0);
    if (!runTests) {
        cout << "Usage: " << argv[0] << " [test]" << endl;
        return 0;
    }

    // Тест 1: конструктор по умолчанию
    matrix m1;
    runTest(1, "default constructor", m1.rows() == 0 && m1.columns() == 0);

    // Тест 2: конструктор nxm
    matrix m2(2, 3);
    runTest(2, "constructor(2,3)", m2.rows() == 2 && m2.columns() == 3);

    // Тест 3: конструктор из double
    matrix m3(5.0);
    runTest(3, "constructor(double)", m3.rows() == 1 && m3.columns() == 1 && m3.get(0,0) == 5.0);

    // Тест 4: конструктор из массива (строка)
    double arr1[] = {1,2,3};
    matrix m4(arr1, 3);
    runTest(4, "constructor(double*,m)", m4.rows() == 1 && m4.columns() == 3 && m4.get(0,0) == 1 && m4.get(0,1) == 2 && m4.get(0,2) == 3);

    // Тест 5: конструктор из массива (столбец)
    double arr2[] = {4,5,6};
    matrix m5(3, arr2);
    runTest(5, "constructor(n,double*)", m5.rows() == 3 && m5.columns() == 1 && m5.get(0,0) == 4 && m5.get(1,0) == 5 && m5.get(2,0) == 6);

    // Тест 6: конструктор из строки
    matrix m6("{{1,0,0},{0,1,0.5}}");
    runTest(6, "constructor(char*)", m6.rows() == 2 && m6.columns() == 3 && m6.get(0,0) == 1 && m6.get(0,1) == 0 && m6.get(0,2) == 0 &&
            m6.get(1,0) == 0 && m6.get(1,1) == 1 && abs(m6.get(1,2) - 0.5) < matrix::EPS);

    // Тест 7: identity
    matrix id = matrix::identity(3);
    runTest(7, "identity(3)", id.rows() == 3 && id.columns() == 3 && id.get(0,0) == 1 && id.get(1,1) == 1 && id.get(2,2) == 1 && id.get(0,1) == 0);

    // Тест 8: diagonal
    double diagVals[] = {1,2,3};
    matrix diag = matrix::diagonal(diagVals, 3);
    runTest(8, "diagonal", diag.rows() == 3 && diag.columns() == 3 && diag.get(0,0) == 1 && diag.get(1,1) == 2 && diag.get(2,2) == 3 && diag.get(0,1) == 0);

    // Тест 9: set/get
    matrix m7(2,2);
    m7.set(0,0, 1.5);
    m7.set(1,1, 2.5);
    runTest(9, "set/get", m7.get(0,0) == 1.5 && m7.get(1,1) == 2.5);

    // Тест 10: operator[] возвращает строку
    matrix m8(2,3);
    m8.set(0,0,1); m8.set(0,1,2); m8.set(0,2,3);
    m8.set(1,0,4); m8.set(1,1,5); m8.set(1,2,6);
    matrix row = m8[0];
    runTest(10, "operator[] row", row.rows() == 1 && row.columns() == 3 && row.get(0,0) == 1 && row.get(0,1) == 2 && row.get(0,2) == 3);

    // Тест 11: сложение матриц
    matrix a(2,2);
    a.set(0,0,1); a.set(0,1,2); a.set(1,0,3); a.set(1,1,4);
    matrix b(2,2);
    b.set(0,0,5); b.set(0,1,6); b.set(1,0,7); b.set(1,1,8);
    matrix c = a + b;
    runTest(11, "operator+", c.get(0,0)==6 && c.get(0,1)==8 && c.get(1,0)==10 && c.get(1,1)==12);

    // Тест 12: вычитание
    matrix d = a - b;
    runTest(12, "operator-", d.get(0,0)==-4 && d.get(0,1)==-4 && d.get(1,0)==-4 && d.get(1,1)==-4);

    // Тест 13: умножение матриц
    matrix e(2,3);
    e.set(0,0,1); e.set(0,1,2); e.set(0,2,3);
    e.set(1,0,4); e.set(1,1,5); e.set(1,2,6);
    matrix f(3,2);
    f.set(0,0,7); f.set(0,1,8);
    f.set(1,0,9); f.set(1,1,10);
    f.set(2,0,11); f.set(2,1,12);
    matrix g = e * f;
    runTest(13, "operator* (matrix)", g.rows() == 2 && g.columns() == 2 && g.get(0,0)==58 && g.get(0,1)==64 && g.get(1,0)==139 && g.get(1,1)==154);

    // Тест 14: умножение на скаляр
    matrix h = a * 2.0;
    runTest(14, "operator* scalar", h.get(0,0)==2 && h.get(0,1)==4 && h.get(1,0)==6 && h.get(1,1)==8);

    // Тест 15: унарный минус
    matrix neg = -a;
    runTest(15, "unary -", neg.get(0,0)==-1 && neg.get(0,1)==-2 && neg.get(1,0)==-3 && neg.get(1,1)==-4);

    // Тест 16: вертикальная конкатенация |
    matrix m9(2,2);
    m9.set(0,0,1); m9.set(0,1,2); m9.set(1,0,3); m9.set(1,1,4);
    matrix m10(2,3);
    m10.set(0,0,5); m10.set(0,1,6); m10.set(0,2,7);
    m10.set(1,0,8); m10.set(1,1,9); m10.set(1,2,10);
    matrix m11 = m9 | m10;
    runTest(16, "vertical concatenation |", m11.rows() == 2 && m11.columns() == 5 &&
            m11.get(0,0)==1 && m11.get(0,1)==2 && m11.get(0,2)==5 && m11.get(0,3)==6 && m11.get(0,4)==7 &&
            m11.get(1,0)==3 && m11.get(1,1)==4 && m11.get(1,2)==8 && m11.get(1,3)==9 && m11.get(1,4)==10);

    // Тест 17: горизонтальная конкатенация /
    matrix m12(2,2);
    m12.set(0,0,1); m12.set(0,1,2); m12.set(1,0,3); m12.set(1,1,4);
    matrix m13(3,2);
    m13.set(0,0,5); m13.set(0,1,6);
    m13.set(1,0,7); m13.set(1,1,8);
    m13.set(2,0,9); m13.set(2,1,10);
    matrix m14 = m12 / m13;
    runTest(17, "horizontal concatenation /", m14.rows() == 5 && m14.columns() == 2 &&
            m14.get(0,0)==1 && m14.get(0,1)==2 &&
            m14.get(1,0)==3 && m14.get(1,1)==4 &&
            m14.get(2,0)==5 && m14.get(2,1)==6 &&
            m14.get(3,0)==7 && m14.get(3,1)==8 &&
            m14.get(4,0)==9 && m14.get(4,1)==10);

    // Тест 18: сравнение ==
    matrix m15(2,2);
    m15.set(0,0,1); m15.set(0,1,2); m15.set(1,0,3); m15.set(1,1,4);
    matrix m16(2,2);
    m16.set(0,0,1); m16.set(0,1,2); m16.set(1,0,3); m16.set(1,1,4);
    runTest(18, "operator==", m15 == m16);

    // Тест 19: сравнение !=
    matrix m17(2,2);
    m17.set(0,0,1); m17.set(0,1,2); m17.set(1,0,3); m17.set(1,1,5);
    runTest(19, "operator!=", m15 != m17);

    // Тест 20: копирование и присваивание
    matrix m18(m15);
    matrix m19;
    m19 = m15;
    runTest(20, "copy constructor and assignment", m18 == m15 && m19 == m15);

    // Тест 21: исключение при несовместимых размерах
    bool caught = false;
    try {
        matrix m20(2,3);
        matrix m21(3,2);
        matrix m22 = m20 + m21;
    } catch (const matrix::error& e) {
        caught = true;
    }
    runTest(21, "exception on incompatible sizes", caught);

    cout << "All tests completed." << endl;
    return 0;
}