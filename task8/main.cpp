#include "Matrix.h"

#include <cstring>
#include <iostream>

static int tests_total = 0;
static int tests_passed = 0;

static void check(bool expr, const char* name) {
    ++tests_total;
    if (expr) {
        ++tests_passed;
        std::cerr << "[OK]   test " << tests_total << ": " << name << "\n";
    } else {
        std::cerr << "[FAIL] test " << tests_total << ": " << name << "\n";
    }
}

static void check_throw_add() {
    ++tests_total;
    try {
        Matrix a(2, 3);
        Matrix b(3, 2);
        Matrix c = a + b;
        (void)c;
        std::cerr << "[FAIL] test " << tests_total << ": incompatible addition must throw\n";
    } catch (const Matrix::error& e) {
        ++tests_passed;
        std::cerr << "[OK]   test " << tests_total << ": incompatible addition throws\n";
        std::cerr << "       " << e.what() << "\n";
    }
}

static void run_tests() {
    try {
        Matrix a(2, 2);
        a.set(1, 1, 1);
        a.set(1, 2, 2);
        a.set(2, 1, 3);
        a.set(2, 2, 4);

        check(a.rows() == 2, "rows()");
        check(a.columns() == 2, "columns()");
        check(static_cast<double>(a[1][1]) == 1.0, "operator[][] read");

        a[1][1] = 10.0;
        check(static_cast<double>(a[1][1]) == 10.0, "operator[][] write");

        Matrix b = a;
        check(b == a, "copy constructor");

        Matrix c(2, 2);
        c = a;
        check(c == a, "assignment operator");

        Matrix d = a * 2.0;
        check(static_cast<double>(d[1][1]) == 20.0, "scalar multiplication");

        Matrix e(2, 2);
        e.set(1, 1, 1);
        e.set(1, 2, 1);
        e.set(2, 1, 1);
        e.set(2, 2, 1);

        Matrix f = a + e;
        check(static_cast<double>(f[1][1]) == 11.0, "operator+");

        Matrix g = a - e;
        check(static_cast<double>(g[2][2]) == 3.0, "operator-");

        Matrix h = a * e;
        check(h.rows() == 2 && h.columns() == 2, "matrix multiplication size");

        Matrix neg = -e;
        check(static_cast<double>(neg[1][1]) == -1.0, "unary minus");

        Matrix id = Matrix::identity(3);
        check(static_cast<double>(id[2][2]) == 1.0, "identity");

        double diag_vals[3] = {5, 6, 7};
        Matrix diag = Matrix::diagonal(diag_vals, 3);
        check(static_cast<double>(diag[3][3]) == 7.0, "diagonal");

        Matrix row = static_cast<Matrix>(a[1]);
        check(row.rows() == 1 && row.columns() == 2, "row slice as matrix");

        const Matrix ca = a;
        check(static_cast<double>(ca[2][2]) == 4.0, "const operator[][]");

        Matrix left(2, 1);
        left.set(1, 1, 1);
        left.set(2, 1, 2);

        Matrix right(2, 1);
        right.set(1, 1, 3);
        right.set(2, 1, 4);

        Matrix concat_h = left | right;
        check(concat_h.rows() == 2 && concat_h.columns() == 2, "operator|");

        Matrix top(1, 2);
        top.set(1, 1, 1);
        top.set(1, 2, 2);

        Matrix bottom(1, 2);
        bottom.set(1, 1, 3);
        bottom.set(1, 2, 4);

        Matrix concat_v = top / bottom;
        check(concat_v.rows() == 2 && concat_v.columns() == 2, "operator/");

        Matrix from_str("1 2; 3 4");
        check(from_str.rows() == 2 && from_str.columns() == 2, "string constructor");
        check(static_cast<double>(from_str[2][1]) == 3.0, "string constructor values");

        check_throw_add();

        // Тесты для my_max()
        {
            Matrix m(2, 3);
            m.set(1, 1, 1); m.set(1, 2, 5); m.set(1, 3, 3);
            m.set(2, 1, 7); m.set(2, 2, 2); m.set(2, 3, 4);
            
            Matrix row_max = m.my_max();
            check(row_max.rows() == 1 && row_max.columns() == 2, "my_max() rows count");
            check(static_cast<double>(row_max[1][1]) == 5.0, "my_max() row1 max");
            check(static_cast<double>(row_max[1][2]) == 7.0, "my_max() row2 max");
            
            Matrix col_max = m.my_max(0);  // фиктивный параметр
            check(col_max.rows() == 1 && col_max.columns() == 3, "my_max(int) columns count");
            check(static_cast<double>(col_max[1][1]) == 7.0, "my_max(int) col1 max");
            check(static_cast<double>(col_max[1][2]) == 5.0, "my_max(int) col2 max");
            check(static_cast<double>(col_max[1][3]) == 4.0, "my_max(int) col3 max");
        }
        // Тест на пустой матрице
        {
            Matrix empty;
            Matrix row_empty = empty.my_max();
            check(row_empty.rows() == 0 && row_empty.columns() == 0, "my_max() on empty");
            Matrix col_empty = empty.my_max(0);
            check(col_empty.rows() == 0 && col_empty.columns() == 0, "my_max(int) on empty");
        }

        std::cerr << "\nPassed " << tests_passed << " of " << tests_total << " tests.\n";
    } catch (const Matrix::error& e) {
        std::cerr << "Unexpected Matrix exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unexpected unknown exception\n";
    }
}


int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "test") == 0) {
        run_tests();
        return 0;
    }

    return 0;
}