#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <stdexcept>

class matrix {
private:
    int rows_;
    int cols_;
    double** data_;

    void allocate(int rows, int cols);
    void deallocate();
    void copyFrom(const matrix& other);

public:
    static const double EPS;

    class error : public std::exception {
    private:
        char msg_[256];
    public:
        error(const char* file, int line, const char* what);
        const char* what() const noexcept override;
    };

    matrix();
    matrix(int n, int m);
    matrix(double d);
    matrix(double* arr, int m);
    matrix(int n, double* arr);
    matrix(const char* str);
    matrix(const matrix& other);
    ~matrix();

    matrix& operator=(const matrix& other);

    static matrix identity(int n);
    static matrix diagonal(double* vals, int n);

    int rows() const { return rows_; }
    int columns() const { return cols_; }
    void set(int i, int j, double val);
    double get(int i, int j) const;

    matrix operator[](int i) const;

    matrix operator+(const matrix& other) const;
    matrix& operator+=(const matrix& other);
    matrix operator-(const matrix& other) const;
    matrix& operator-=(const matrix& other);
    matrix operator*(const matrix& other) const;
    matrix& operator*=(const matrix& other);
    matrix operator*(double scalar) const;
    friend matrix operator*(double scalar, const matrix& m);
    matrix& operator*=(double scalar);
    matrix operator-() const;

    matrix operator|(const matrix& other) const;
    matrix operator/(const matrix& other) const;

    bool operator==(const matrix& other) const;
    bool operator!=(const matrix& other) const;

    friend std::ostream& operator<<(std::ostream& os, const matrix& m);
};

#endif