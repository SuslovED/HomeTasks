#include "matrix.h"
#include <cstring>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

const double matrix::EPS = 1e-9;

matrix::error::error(const char* file, int line, const char* what) {
    std::snprintf(msg_, sizeof(msg_), "[%s:%d] %s", file, line, what);
}

const char* matrix::error::what() const noexcept {
    return msg_;
}

void matrix::allocate(int rows, int cols) {
    rows_ = rows;
    cols_ = cols;
    data_ = new double*[rows_];
    for (int i = 0; i < rows_; ++i) {
        data_[i] = new double[cols_]();
    }
}

void matrix::deallocate() {
    if (data_) {
        for (int i = 0; i < rows_; ++i) {
            delete[] data_[i];
        }
        delete[] data_;
        data_ = nullptr;
    }
    rows_ = 0;
    cols_ = 0;
}

void matrix::copyFrom(const matrix& other) {
    allocate(other.rows_, other.cols_);
    for (int i = 0; i < rows_; ++i) {
        std::memcpy(data_[i], other.data_[i], cols_ * sizeof(double));
    }
}

matrix::matrix() : rows_(0), cols_(0), data_(nullptr) {}

matrix::matrix(int n, int m) {
    if (n < 0 || m < 0) throw error(__FILE__, __LINE__, "negative dimensions");
    allocate(n, m);
}

matrix::matrix(double d) {
    allocate(1, 1);
    data_[0][0] = d;
}

matrix::matrix(double* arr, int m) {
    if (m < 0) throw error(__FILE__, __LINE__, "negative columns");
    allocate(1, m);
    std::memcpy(data_[0], arr, m * sizeof(double));
}

matrix::matrix(int n, double* arr) {
    if (n < 0) throw error(__FILE__, __LINE__, "negative rows");
    allocate(n, 1);
    for (int i = 0; i < n; ++i) {
        data_[i][0] = arr[i];
    }
}

matrix::matrix(const char* str) {
    if (!str || !*str) throw error(__FILE__, __LINE__, "empty string");

    size_t len = std::strlen(str);
    if (str[0] != '{' || str[len-1] != '}')
        throw error(__FILE__, __LINE__, "matrix must be enclosed in curly braces");

    // Пропускаем первую {
    const char* p = str + 1;

    // Подсчёт строк
    int rows = 0;
    const char* q = p;
    while (*q) {
        if (*q == '{') {
            rows++;
            while (*q && *q != '}') q++;
            if (*q == '}') q++;
        } else {
            q++;
        }
    }
    if (rows == 0) throw error(__FILE__, __LINE__, "no rows found");

    // Определение числа столбцов по первой строке
    q = p;
    while (*q && *q != '{') q++;
    if (*q != '{') throw error(__FILE__, __LINE__, "invalid format");
    const char* startRow = q + 1;
    const char* endRow = startRow;
    while (*endRow && *endRow != '}') endRow++;
    if (*endRow != '}') throw error(__FILE__, __LINE__, "unclosed row");

    int cols = 0;
    const char* r = startRow;
    bool inNumber = false;
    while (r < endRow) {
        if (std::isdigit(*r) || *r == '.' || *r == '-' || *r == '+') {
            if (!inNumber) {
                inNumber = true;
                cols++;
            }
        } else if (*r == ',' || *r == ' ') {
            inNumber = false;
        }
        r++;
    }
    if (cols == 0) throw error(__FILE__, __LINE__, "no columns");

    // Выделение памяти
    allocate(rows, cols);

    // Заполнение матрицы
    int row = 0;
    q = p;
    while (*q && row < rows) {
        while (*q && *q != '{') q++;
        if (*q != '{') break;
        const char* rowStart = q + 1;
        const char* rowEnd = rowStart;
        while (*rowEnd && *rowEnd != '}') rowEnd++;
        if (*rowEnd != '}') throw error(__FILE__, __LINE__, "unclosed row");

        int col = 0;
        const char* r = rowStart;
        inNumber = false;
        while (r < rowEnd) {
            if (std::isdigit(*r) || *r == '.' || *r == '-' || *r == '+') {
                if (!inNumber) {
                    inNumber = true;
                    if (col >= cols) throw error(__FILE__, __LINE__, "too many columns");
                    char* end;
                    double val = std::strtod(r, &end);
                    if (end == r) throw error(__FILE__, __LINE__, "invalid number");
                    data_[row][col++] = val;
                    r = end;
                    continue;
                }
            } else if (*r == ',' || *r == ' ') {
                inNumber = false;
            }
            r++;
        }
        if (col != cols) throw error(__FILE__, __LINE__, "column count mismatch");
        row++;
        q = rowEnd + 1; // переходим за }
    }
    if (row != rows) throw error(__FILE__, __LINE__, "row count mismatch");
}

matrix::matrix(const matrix& other) : rows_(0), cols_(0), data_(nullptr) {
    copyFrom(other);
}

matrix::~matrix() {
    deallocate();
}

matrix& matrix::operator=(const matrix& other) {
    if (this != &other) {
        deallocate();
        copyFrom(other);
    }
    return *this;
}

matrix matrix::identity(int n) {
    if (n < 0) throw error(__FILE__, __LINE__, "negative size");
    matrix res(n, n);
    for (int i = 0; i < n; ++i) res.data_[i][i] = 1.0;
    return res;
}

matrix matrix::diagonal(double* vals, int n) {
    if (n < 0) throw error(__FILE__, __LINE__, "negative size");
    matrix res(n, n);
    for (int i = 0; i < n; ++i) res.data_[i][i] = vals[i];
    return res;
}

void matrix::set(int i, int j, double val) {
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_)
        throw error(__FILE__, __LINE__, "index out of range");
    data_[i][j] = val;
}

double matrix::get(int i, int j) const {
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_)
        throw error(__FILE__, __LINE__, "index out of range");
    return data_[i][j];
}

matrix matrix::operator[](int i) const {
    if (i < 0 || i >= rows_)
        throw error(__FILE__, __LINE__, "row index out of range");
    matrix row(1, cols_);
    std::memcpy(row.data_[0], data_[i], cols_ * sizeof(double));
    return row;
}

matrix matrix::operator+(const matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_)
        throw error(__FILE__, __LINE__, "incompatible sizes for addition");
    matrix res(rows_, cols_);
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            res.data_[i][j] = data_[i][j] + other.data_[i][j];
    return res;
}

matrix& matrix::operator+=(const matrix& other) {
    *this = *this + other;
    return *this;
}

matrix matrix::operator-(const matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_)
        throw error(__FILE__, __LINE__, "incompatible sizes for subtraction");
    matrix res(rows_, cols_);
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            res.data_[i][j] = data_[i][j] - other.data_[i][j];
    return res;
}

matrix& matrix::operator-=(const matrix& other) {
    *this = *this - other;
    return *this;
}

matrix matrix::operator*(const matrix& other) const {
    if (cols_ != other.rows_)
        throw error(__FILE__, __LINE__, "incompatible sizes for multiplication");
    matrix res(rows_, other.cols_);
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < other.cols_; ++j) {
            double sum = 0.0;
            for (int k = 0; k < cols_; ++k) {
                sum += data_[i][k] * other.data_[k][j];
            }
            res.data_[i][j] = sum;
        }
    }
    return res;
}

matrix& matrix::operator*=(const matrix& other) {
    *this = *this * other;
    return *this;
}

matrix matrix::operator*(double scalar) const {
    matrix res(rows_, cols_);
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            res.data_[i][j] = data_[i][j] * scalar;
    return res;
}

matrix operator*(double scalar, const matrix& m) {
    return m * scalar;
}

matrix& matrix::operator*=(double scalar) {
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            data_[i][j] *= scalar;
    return *this;
}

matrix matrix::operator-() const {
    matrix res(rows_, cols_);
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            res.data_[i][j] = -data_[i][j];
    return res;
}

matrix matrix::operator|(const matrix& other) const {
    if (rows_ != other.rows_)
        throw error(__FILE__, __LINE__, "incompatible rows for vertical concatenation");
    matrix res(rows_, cols_ + other.cols_);
    for (int i = 0; i < rows_; ++i) {
        std::memcpy(res.data_[i], data_[i], cols_ * sizeof(double));
        std::memcpy(res.data_[i] + cols_, other.data_[i], other.cols_ * sizeof(double));
    }
    return res;
}

matrix matrix::operator/(const matrix& other) const {
    if (cols_ != other.cols_)
        throw error(__FILE__, __LINE__, "incompatible columns for horizontal concatenation");
    matrix res(rows_ + other.rows_, cols_);
    for (int i = 0; i < rows_; ++i) {
        std::memcpy(res.data_[i], data_[i], cols_ * sizeof(double));
    }
    for (int i = 0; i < other.rows_; ++i) {
        std::memcpy(res.data_[rows_ + i], other.data_[i], cols_ * sizeof(double));
    }
    return res;
}

bool matrix::operator==(const matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) return false;
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            if (std::abs(data_[i][j] - other.data_[i][j]) > EPS)
                return false;
    return true;
}

bool matrix::operator!=(const matrix& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const matrix& m) {
    for (int i = 0; i < m.rows_; ++i) {
        for (int j = 0; j < m.cols_; ++j) {
            os << m.data_[i][j];
            if (j != m.cols_ - 1) os << ' ';
        }
        os << '\n';
    }
    return os;
}