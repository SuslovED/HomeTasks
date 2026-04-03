#include "Matrix.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

#define MATRIX_THROW(msg) throw Matrix::error((msg), __LINE__)

const double Matrix::EPS = 1e-9;

Matrix::error::error(const char* text, int line) {
    std::snprintf(message_, sizeof(message_), "Matrix error at line %d: %s", line, text);
}

const char* Matrix::error::what() const {
    return message_;
}

void Matrix::allocate(int r, int c) {
    if (r < 0 || c < 0) {
        MATRIX_THROW("negative size");
    }

    rows_ = r;
    cols_ = c;

    if (rows_ == 0 || cols_ == 0) {
        data_ = 0;
        return;
    }

    data_ = new double[rows_ * cols_];
}

void Matrix::destroy() {
    delete[] data_;
    data_ = 0;
    rows_ = 0;
    cols_ = 0;
}

int Matrix::index(int i, int j) const {
    return (i - 1) * cols_ + (j - 1);
}

void Matrix::check_matrix_index(int i, int j) const {
    if (i < 1 || i > rows_ || j < 1 || j > cols_) {
        MATRIX_THROW("index out of range");
    }
}

void Matrix::check_slice_index(int i) const {
    if (i < 1 || (i > rows_ && i > cols_)) {
        MATRIX_THROW("slice index out of range");
    }
}

Matrix::Matrix() : rows_(0), cols_(0), data_(0) {}

Matrix::Matrix(int n, int m) : rows_(0), cols_(0), data_(0) {
    if (n <= 0 || m <= 0) {
        MATRIX_THROW("invalid matrix size");
    }

    allocate(n, m);
    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] = 0.0;
    }
}

Matrix::Matrix(double value) : rows_(0), cols_(0), data_(0) {
    allocate(1, 1);
    data_[0] = value;
}

Matrix::Matrix(double* values, int m) : rows_(0), cols_(0), data_(0) {
    if (values == 0 || m <= 0) {
        MATRIX_THROW("invalid row constructor arguments");
    }

    allocate(1, m);
    for (int j = 1; j <= m; ++j) {
        set(1, j, values[j - 1]);
    }
}

Matrix::Matrix(int n, double* values) : rows_(0), cols_(0), data_(0) {
    if (values == 0 || n <= 0) {
        MATRIX_THROW("invalid column constructor arguments");
    }

    allocate(n, 1);
    for (int i = 1; i <= n; ++i) {
        set(i, 1, values[i - 1]);
    }
}

Matrix::Matrix(const char* text) : rows_(0), cols_(0), data_(0) {
    if (text == 0) {
        MATRIX_THROW("null string");
    }

    // Формат: "1 2 3; 4 5 6; 7 8 9"
    int rows = 0;
    int cols = -1;

    const char* p = text;
    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            ++p;
        }

        if (*p == '\0') {
            break;
        }

        int current_cols = 0;
        while (*p != '\0' && *p != ';') {
            char* endptr = 0;
            std::strtod(p, &endptr);
            if (endptr == p) {
                MATRIX_THROW("invalid matrix string");
            }
            ++current_cols;
            p = endptr;

            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
                ++p;
            }
        }

        if (current_cols == 0) {
            MATRIX_THROW("empty row in matrix string");
        }

        if (cols == -1) {
            cols = current_cols;
        } else if (cols != current_cols) {
            MATRIX_THROW("rows have different lengths");
        }

        ++rows;

        if (*p == ';') {
            ++p;
        }
    }

    if (rows <= 0 || cols <= 0) {
        MATRIX_THROW("empty matrix string");
    }

    allocate(rows, cols);

    p = text;
    int i = 1;
    int j = 1;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            ++p;
        }

        if (*p == '\0') {
            break;
        }

        if (*p == ';') {
            ++i;
            j = 1;
            ++p;
            continue;
        }

        char* endptr = 0;
        double value = std::strtod(p, &endptr);
        if (endptr == p) {
            MATRIX_THROW("invalid number in matrix string");
        }

        set(i, j, value);
        ++j;
        p = endptr;
    }
}

Matrix::Matrix(const Matrix& other) : rows_(0), cols_(0), data_(0) {
    allocate(other.rows_, other.cols_);
    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] = other.data_[i];
    }
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }

    destroy();
    allocate(other.rows_, other.cols_);
    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] = other.data_[i];
    }

    return *this;
}

Matrix::~Matrix() {
    destroy();
}

Matrix Matrix::identity(int n) {
    if (n <= 0) {
        MATRIX_THROW("invalid identity size");
    }

    Matrix result(n, n);
    for (int i = 1; i <= n; ++i) {
        result.set(i, i, 1.0);
    }
    return result;
}

Matrix Matrix::diagonal(double* vals, int n) {
    if (vals == 0 || n <= 0) {
        MATRIX_THROW("invalid diagonal arguments");
    }

    Matrix result(n, n);
    for (int i = 1; i <= n; ++i) {
        result.set(i, i, vals[i - 1]);
    }
    return result;
}

int Matrix::rows() const {
    return rows_;
}

int Matrix::columns() const {
    return cols_;
}

void Matrix::set(int i, int j, double value) {
    check_matrix_index(i, j);
    data_[index(i, j)] = value;
}

double Matrix::get(int i, int j) const {
    check_matrix_index(i, j);
    return data_[index(i, j)];
}

Matrix::SliceProxy Matrix::operator[](int i) {
    check_slice_index(i);
    return SliceProxy(this, i);
}

Matrix::ConstSliceProxy Matrix::operator[](int i) const {
    check_slice_index(i);
    return ConstSliceProxy(this, i);
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(*this);
    result *= scalar;
    return result;
}

Matrix& Matrix::operator*=(double scalar) {
    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] *= scalar;
    }
    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        MATRIX_THROW("incompatible sizes for +");
    }

    Matrix result(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; ++i) {
        result.data_[i] = data_[i] + other.data_[i];
    }
    return result;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        MATRIX_THROW("incompatible sizes for +=");
    }

    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] += other.data_[i];
    }
    return *this;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        MATRIX_THROW("incompatible sizes for -");
    }

    Matrix result(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; ++i) {
        result.data_[i] = data_[i] - other.data_[i];
    }
    return result;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        MATRIX_THROW("incompatible sizes for -=");
    }

    for (int i = 0; i < rows_ * cols_; ++i) {
        data_[i] -= other.data_[i];
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols_ != other.rows_) {
        MATRIX_THROW("incompatible sizes for matrix multiplication");
    }

    Matrix result(rows_, other.cols_);

    for (int i = 1; i <= rows_; ++i) {
        for (int j = 1; j <= other.cols_; ++j) {
            double sum = 0.0;
            for (int k = 1; k <= cols_; ++k) {
                sum += get(i, k) * other.get(k, j);
            }
            result.set(i, j, sum);
        }
    }

    return result;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    Matrix tmp = (*this) * other;
    *this = tmp;
    return *this;
}

Matrix Matrix::operator-() const {
    Matrix result(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; ++i) {
        result.data_[i] = -data_[i];
    }
    return result;
}

bool Matrix::operator==(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        return false;
    }

    for (int i = 0; i < rows_ * cols_; ++i) {
        if (std::fabs(data_[i] - other.data_[i]) > EPS) {
            return false;
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& other) const {
    return !((*this) == other);
}

// По скобкам из ТЗ: вторую справа от первой
Matrix Matrix::operator|(const Matrix& other) const {
    if (rows_ != other.rows_) {
        MATRIX_THROW("incompatible sizes for |");
    }

    Matrix result(rows_, cols_ + other.cols_);
    for (int i = 1; i <= rows_; ++i) {
        for (int j = 1; j <= cols_; ++j) {
            result.set(i, j, get(i, j));
        }
        for (int j = 1; j <= other.cols_; ++j) {
            result.set(i, cols_ + j, other.get(i, j));
        }
    }
    return result;
}

// По скобкам из ТЗ: вторую под первой
Matrix Matrix::operator/(const Matrix& other) const {
    if (cols_ != other.cols_) {
        MATRIX_THROW("incompatible sizes for /");
    }

    Matrix result(rows_ + other.rows_, cols_);
    for (int i = 1; i <= rows_; ++i) {
        for (int j = 1; j <= cols_; ++j) {
            result.set(i, j, get(i, j));
        }
    }
    for (int i = 1; i <= other.rows_; ++i) {
        for (int j = 1; j <= cols_; ++j) {
            result.set(rows_ + i, j, other.get(i, j));
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const Matrix& m) {
    for (int i = 1; i <= m.rows_; ++i) {
        for (int j = 1; j <= m.cols_; ++j) {
            out << m.get(i, j);
            if (j != m.cols_) {
                out << ' ';
            }
        }
        if (i != m.rows_) {
            out << '\n';
        }
    }
    return out;
}

Matrix::ElementProxy::ElementProxy(Matrix* owner, int first, int second, bool row_mode)
    : owner_(owner), first_(first), second_(second), row_mode_(row_mode) {}

Matrix::ElementProxy& Matrix::ElementProxy::operator=(double value) {
    if (row_mode_) {
        owner_->set(first_, second_, value);
    } else {
        owner_->set(second_, first_, value);
    }
    return *this;
}

Matrix::ElementProxy& Matrix::ElementProxy::operator=(const ElementProxy& other) {
    return (*this = static_cast<double>(other));
}

Matrix::ElementProxy::operator double() const {
    if (row_mode_) {
        return owner_->get(first_, second_);
    }
    return owner_->get(second_, first_);
}

Matrix::ElementProxy::operator Matrix() const {
    return Matrix(static_cast<double>(*this));
}

Matrix::ConstElementProxy::ConstElementProxy(const Matrix* owner, int first, int second, bool row_mode)
    : owner_(owner), first_(first), second_(second), row_mode_(row_mode) {}

Matrix::ConstElementProxy::operator double() const {
    if (row_mode_) {
        return owner_->get(first_, second_);
    }
    return owner_->get(second_, first_);
}

Matrix::ConstElementProxy::operator Matrix() const {
    return Matrix(static_cast<double>(*this));
}

Matrix::SliceProxy::SliceProxy(Matrix* owner, int pos)
    : owner_(owner), pos_(pos) {}

Matrix::ElementProxy Matrix::SliceProxy::operator[](int j) {
    if (pos_ <= owner_->rows_) {
        if (j < 1 || j > owner_->cols_) {
            MATRIX_THROW("column index out of range");
        }
        return ElementProxy(owner_, pos_, j, true);
    }

    if (pos_ <= owner_->cols_) {
        if (j < 1 || j > owner_->rows_) {
            MATRIX_THROW("row index out of range");
        }
        return ElementProxy(owner_, pos_, j, false);
    }

    MATRIX_THROW("slice index out of range");
}

Matrix::SliceProxy::operator Matrix() const {
    if (pos_ <= owner_->rows_) {
        Matrix result(1, owner_->cols_);
        for (int j = 1; j <= owner_->cols_; ++j) {
            result.set(1, j, owner_->get(pos_, j));
        }
        return result;
    }

    if (pos_ <= owner_->cols_) {
        Matrix result(owner_->rows_, 1);
        for (int i = 1; i <= owner_->rows_; ++i) {
            result.set(i, 1, owner_->get(i, pos_));
        }
        return result;
    }

    MATRIX_THROW("slice index out of range");
}

Matrix::ConstSliceProxy::ConstSliceProxy(const Matrix* owner, int pos)
    : owner_(owner), pos_(pos) {}

Matrix::ConstElementProxy Matrix::ConstSliceProxy::operator[](int j) const {
    if (pos_ <= owner_->rows_) {
        if (j < 1 || j > owner_->cols_) {
            MATRIX_THROW("column index out of range");
        }
        return ConstElementProxy(owner_, pos_, j, true);
    }

    if (pos_ <= owner_->cols_) {
        if (j < 1 || j > owner_->rows_) {
            MATRIX_THROW("row index out of range");
        }
        return ConstElementProxy(owner_, pos_, j, false);
    }

    MATRIX_THROW("slice index out of range");
}

Matrix::ConstSliceProxy::operator Matrix() const {
    if (pos_ <= owner_->rows_) {
        Matrix result(1, owner_->cols_);
        for (int j = 1; j <= owner_->cols_; ++j) {
            result.set(1, j, owner_->get(pos_, j));
        }
        return result;
    }

    if (pos_ <= owner_->cols_) {
        Matrix result(owner_->rows_, 1);
        for (int i = 1; i <= owner_->rows_; ++i) {
            result.set(i, 1, owner_->get(i, pos_));
        }
        return result;
    }

    MATRIX_THROW("slice index out of range");
}

Matrix Matrix::my_max() const {
    // Максимумы по строкам → матрица 1×rows
    if (rows_ == 0 || cols_ == 0) {
        return Matrix();   // пустая матрица
    }
    Matrix result(1, rows_);
    for (int i = 1; i <= rows_; ++i) {
        double max_val = get(i, 1);
        for (int j = 2; j <= cols_; ++j) {
            double v = get(i, j);
            if (v > max_val) max_val = v;
        }
        result.set(1, i, max_val);
    }
    return result;
}

Matrix Matrix::my_max(int) const {
    // Максимумы по столбцам → матрица 1×cols
    if (rows_ == 0 || cols_ == 0) {
        return Matrix();   // пустая матрица
    }
    Matrix result(1, cols_);
    for (int j = 1; j <= cols_; ++j) {
        double max_val = get(1, j);
        for (int i = 2; i <= rows_; ++i) {
            double v = get(i, j);
            if (v > max_val) max_val = v;
        }
        result.set(1, j, max_val);
    }
    return result;
}