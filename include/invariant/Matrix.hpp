#ifndef INVARIANT_MATRIX_HPP
#define INVARIANT_MATRIX_HPP

#include <iostream>
#include <random>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <string>
#include <algorithm>
#include <typeinfo>

namespace invariant {

template<typename T>
class Matrix {
private:
    size_t rows, cols;
    T* data;
public:
    // Default constructor
    Matrix() : rows(0), cols(0), data(nullptr) {}
    // Constructor
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data = new T[rows * cols]();
    }
    // Constructor with initial value
    Matrix(size_t r, size_t c, T initialValue) : rows(r), cols(c) {
        data = new T[rows * cols];
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = initialValue;
        }
    }
    // Constructor with Array of Arrays
    Matrix(size_t r, size_t c, T** array) : rows(r), cols(c) {
        data = new T[rows * cols];
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i * cols + j] = array[i][j];
            }
        }
    }
    // Constructor of random values in range
    Matrix(size_t r, size_t c, T minValue, T maxValue) : rows(r), cols(c) {
        data = new T[rows * cols];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(minValue, maxValue);
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = static_cast<T>(dis(gen));
        }
    }
    // Copy constructor
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
        data = new T[rows * cols];
        for (size_t i = 0; i < rows * cols; ++i) {
            data[i] = other.data[i];
        }
    }
    // Copy assignment operator
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            delete[] data;
            rows = other.rows;
            cols = other.cols;
            data = new T[rows * cols];
            for (size_t i = 0; i < rows * cols; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    // Move constructor
    Matrix(Matrix&& other) noexcept : rows(other.rows), cols(other.cols), data(other.data) {
        other.data = nullptr;
        other.rows = 0;
        other.cols = 0;
    }
    // Move assignment operator
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            delete[] data;
            rows = other.rows;
            cols = other.cols;
            data = other.data;
            other.data = nullptr;
            other.rows = 0;
            other.cols = 0;
        }
        return *this;
    }
    // Destructor
    ~Matrix() {
        delete[] data;
    }
    // Access element
    T& at(size_t r, size_t c) {
        return data[r * cols + c];
    }
    // Const access element
    const T& at(size_t r, size_t c) const {
        return data[r * cols + c];
    }
    // Getters for rows and columns
    size_t getRows() const {
        return rows;
    }
    size_t getCols() const {
        return cols;
    }
    // Print matrix
    void print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                 std::cout << data[i * cols + j] << " ";
            }
             std::cout << std::endl;
        }
    }
    // Multiply by a matrix
    Matrix operator*(const Matrix& other) const {
        if (this->cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
        }
        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                T sum = T();
                for (size_t k = 0; k < cols; ++k) {
                    sum += at(i, k) * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }
    // Multiply by a scalar
    Matrix operator*(T scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; ++i) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }
    // Transpose
    Matrix transpose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.at(j, i) = at(i, j);
            }
        }
        return result;
    }
    // Reciprocal of all elements, make each element 1/element
    Matrix reciprocal() const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; ++i) {
            if (data[i] == T()) {
                throw std::invalid_argument("Division by zero in reciprocal.");
            }
            result.data[i] = T(1) / data[i];
        }
        return result;
    }
    // print dimensions
    void printDim() const {
         std::cout << "Rows: " << rows << ", Cols: " << cols << std::endl;
    }
    // get dimensions
    std::string dim() const {
        return std::to_string(rows) + "x" + std::to_string(cols);
    }
    // Make column-stochastic, meaning the columns sum to 1
    void makeColumnStochastic() {
        Matrix ones = Matrix(rows, 1, T(1));
        Matrix colSums = (*this).transpose() * ones;
        for (size_t j = 0; j < cols; ++j) {
            T colSum = colSums.at(j, 0);
            if (colSum == T()) {
                throw std::invalid_argument("Column sum is zero, cannot make stochastic.");
            }
            for (size_t i = 0; i < rows; ++i) {
                at(i, j) /= colSum;
            }
        }

        *this = *this;
    }
    // Solve a linear system Ax = b using Gaussian elimination
    Matrix solve(const Matrix& b) const {
        if (rows != cols || b.rows != rows || b.cols != 1) {
            throw std::invalid_argument("Invalid dimensions for solving linear system.");
        }
        Matrix A(*this);
        Matrix x(b);
        for (size_t i = 0; i < rows; ++i) {
            // Pivoting
            size_t maxRow = i;
            for (size_t k = i + 1; k < rows; ++k) {
                if (std::abs(A.at(k, i)) > std::abs(A.at(maxRow, i))) {
                    maxRow = k;
                }
            }
            for (size_t k = i; k < cols; ++k) {
                std::swap(A.at(i, k), A.at(maxRow, k));
            }
            std::swap(x.at(i, 0), x.at(maxRow, 0));
            // Elimination
            for (size_t k = i + 1; k < rows; ++k) {
                T factor = A.at(k, i) / A.at(i, i);
                for (size_t j = i; j < cols; ++j) {
                    A.at(k, j) -= factor * A.at(i, j);
                }
                x.at(k, 0) -= factor * x.at(i, 0);
            }
        }
        // Back substitution
        Matrix solution(rows, 1);
        for (int i = rows - 1; i >= 0; --i) {
            T sum = x.at(i, 0);
            for (size_t j = i + 1; j < cols; ++j) {
                sum -= A.at(i, j) * solution.at(j, 0);
            }
            solution.at(i, 0) = sum / A.at(i, i);
        }
        return solution;
    }

    Matrix LinearFit(T* x_values, T* y_values, size_t n) {
        if (n < 2) {
            throw std::invalid_argument("Invalid input sizes for linear fit.");
        }
        Matrix<T> X(n, n);
        Matrix<T> Y(n, 1);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                X.at(i, j) = std::pow(x_values[i], j);
            }
        }
        for (size_t i = 0; i < n; ++i) {
            Y.at(i, 0) = y_values[i];
        }
        Matrix<T> coeffs = X.solve(Y);
        // print equation
        for (size_t i = 0; i < coeffs.getRows(); ++i) {
             std::cout << coeffs.at(i, 0);
            if (i > 0) {
                 std::cout << "x^" << i;
            }
            if (i < coeffs.getRows() - 1) {
                 std::cout << " + ";
            }
        }
         std::cout << std::endl;
        return coeffs;
    }
    // Non-mutable power function using exponentiation by squaring
    Matrix<T> power(int exponent) {
        if (rows != cols) {
            throw std::invalid_argument("Matrix must be square to raise to a power.");
        }
        Matrix<T> result(rows, cols, T(0));
        for (size_t i = 0; i < rows; ++i) {
            result.at(i, i) = T(1);
        }
        Matrix<T> base(*this);
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = result * base;
            }
            base = base * base;
            exponent /= 2;
        }
        return result;
    }

    void display() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (typeid(T) == typeid(double)) {
                     std::cout << std::fixed << std::setprecision(18) << at(i, j) << " ";
                } else {
                     std::cout << at(i, j) << " ";
                }
            }
        }
         std::cout << std::endl;
    }

    Matrix Add(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions do not match for addition.");
        }
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Matrix Subtract(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions do not match for subtraction.");
        }
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows * cols; ++i) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    double two_norm_euclidian_length_difference(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions do not match for norm calculation.");
        }
        double sum = 0.0;
        for (size_t i = 0; i < rows * cols; ++i) {
            double diff = static_cast<double>(data[i]) - static_cast<double>(other.data[i]);
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }

    double sum() const {
        double total = 0.0;
        for (size_t i = 0; i < rows * cols; ++i) {
            total += static_cast<double>(data[i]);
        }
        return total;
    }

    void displayAsPolynomial() const {
        for (size_t i = 0; i < cols; ++i) {
             std::cout << at(0, i);
            if (i > 0) {
                 std::cout << "x^" << i;
            }
            if (i < cols - 1) {
                 std::cout << " + ";
            }
        }
         std::cout << std::endl;
    }
};

}

#endif  // INVARIANT_MATRIX_HPP