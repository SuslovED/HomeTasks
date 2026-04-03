#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

class Matrix {
public:
    class error {
    private:
        char message_[256];
    public:
        error(const char* text, int line);
        const char* what() const;
    };

    static const double EPS;

private:
    int rows_;
    int cols_;
    double* data_;

    void allocate(int r, int c);
    void destroy();
    int index(int i, int j) const;
    void check_matrix_index(int i, int j) const;
    void check_slice_index(int i) const;

public:
    Matrix my_max() const;
    Matrix my_max(int) const;

    class ElementProxy;
    class ConstElementProxy;
    class SliceProxy;
    class ConstSliceProxy;

    Matrix();
    Matrix(int n, int m);
    Matrix(double value);
    Matrix(double* values, int m);
    Matrix(int n, double* values);
    Matrix(const char* text);
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    ~Matrix();

    static Matrix identity(int n);
    static Matrix diagonal(double* vals, int n);

    int rows() const;
    int columns() const;

    void set(int i, int j, double value);
    double get(int i, int j) const;

    SliceProxy operator[](int i);
    ConstSliceProxy operator[](int i) const;

    Matrix operator*(double scalar) const;
    Matrix& operator*=(double scalar);

    Matrix operator+(const Matrix& other) const;
    Matrix& operator+=(const Matrix& other);

    Matrix operator-(const Matrix& other) const;
    Matrix& operator-=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;
    Matrix& operator*=(const Matrix& other);

    Matrix operator-() const;

    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

    Matrix operator|(const Matrix& other) const; // справа
    Matrix operator/(const Matrix& other) const; // снизу

    friend std::ostream& operator<<(std::ostream& out, const Matrix& m);

public:
    class ElementProxy {
    private:
        Matrix* owner_;
        int first_;
        int second_;
        bool row_mode_;

    public:
        ElementProxy(Matrix* owner, int first, int second, bool row_mode);
        ElementProxy(const ElementProxy&) = default;
        ElementProxy& operator=(double value);
        ElementProxy& operator=(const ElementProxy& other);
        operator double() const;
        operator Matrix() const;
    };

    class ConstElementProxy {
    private:
        const Matrix* owner_;
        int first_;
        int second_;
        bool row_mode_;

    public:
        ConstElementProxy(const Matrix* owner, int first, int second, bool row_mode);
        operator double() const;
        operator Matrix() const;
    };

    class SliceProxy {
    private:
        Matrix* owner_;
        int pos_;

    public:
        SliceProxy(Matrix* owner, int pos);
        ElementProxy operator[](int j);
        operator Matrix() const;
    };

    class ConstSliceProxy {
    private:
        const Matrix* owner_;
        int pos_;

    public:
        ConstSliceProxy(const Matrix* owner, int pos);
        ConstElementProxy operator[](int j) const;
        operator Matrix() const;
    };
};

#endif