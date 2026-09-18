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
#include <vector>
#include <limits>
#include <type_traits>
#include <utility>

namespace invariant {

template<typename T>
class Matrix {
private:
    size_t rows, cols;
    std::vector<T> data;
    static size_t checked_size(size_t r, size_t c) {
        if (c && r > std::vector<T>().max_size() / c)
            throw std::length_error("Matrix dimensions exceed storage capacity.");
        return r * c;
    }
    void require_finite() const {
        for (const auto value : data)
            if (!std::isfinite(value))
                throw std::invalid_argument("Matrix contains non-finite values.");
    }
public:
    Matrix() : rows(0), cols(0) {}
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(checked_size(r, c), T{}) {}
    Matrix(size_t r, size_t c, T value) : rows(r), cols(c), data(checked_size(r, c), value) {}
    Matrix(size_t r, size_t c, T** array) : Matrix(r, c) {
        if (r && c && !array) throw std::invalid_argument("Null matrix array.");
        for (size_t i = 0; i < r && c; ++i) {
            if (!array[i]) throw std::invalid_argument("Null matrix row.");
            std::copy_n(array[i], c, data.begin() + i * c);
        }
    }
    Matrix(size_t r, size_t c, T minValue, T maxValue) : Matrix(r, c) {
        if (!std::isfinite(minValue) || !std::isfinite(maxValue) || minValue > maxValue)
            throw std::invalid_argument("Invalid random range.");
        std::mt19937 gen(std::random_device{}());
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dis(minValue, maxValue);
            for (auto& value : data) value = dis(gen);
        } else {
            std::uniform_real_distribution<T> dis(minValue, maxValue);
            for (auto& value : data) value = dis(gen);
        }
    }
    Matrix(const Matrix&) = default;
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            Matrix copy(other);
            swap(copy);
        }
        return *this;
    }
    Matrix(Matrix&& other) noexcept : Matrix() { swap(other); }
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            Matrix moved(std::move(other));
            swap(moved);
        }
        return *this;
    }
    void swap(Matrix& other) noexcept {
        std::swap(rows, other.rows);
        std::swap(cols, other.cols);
        data.swap(other.data);
    }
    // Access element
    T& at(size_t r, size_t c) {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of bounds.");
        return data[r * cols + c];
    }
    // Const access element
    const T& at(size_t r, size_t c) const {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of bounds.");
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
        for (size_t i = 0; i < rows; ++i)
            for (size_t k = 0; k < cols; ++k) {
                const T value = data[i * cols + k];
                for (size_t j = 0; j < other.cols; ++j)
                    result.data[i * other.cols + j] += value * other.data[k * other.cols + j];
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
        static_assert(std::is_floating_point_v<T>, "Normalization requires floating point.");
        require_finite();
        std::vector<T> sums(cols, T{});
        for (size_t j = 0; j < cols; ++j) {
            for (size_t i = 0; i < rows; ++i) {
                if (at(i, j) < T{}) throw std::invalid_argument("Stochastic entries must be nonnegative.");
                sums[j] += at(i, j);
            }
            if (!(sums[j] > T{}) || !std::isfinite(sums[j]))
                throw std::invalid_argument("Column sum must be positive and finite.");
        }
        for (size_t j = 0; j < cols; ++j)
            for (size_t i = 0; i < rows; ++i) at(i, j) /= sums[j];
    }
    // Gaussian elimination with scaled partial pivoting; supports multiple RHS columns.
    Matrix solve(const Matrix& b) const {
        static_assert(std::is_floating_point_v<T>, "Solving requires floating point.");
        if (!rows || rows != cols || b.rows != rows || !b.cols)
            throw std::invalid_argument("Invalid dimensions for solving linear system.");
        require_finite();
        b.require_finite();
        Matrix A(*this), x(b);
        std::vector<T> scale(rows, T{});
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) scale[i] = std::max(scale[i], std::abs(A.at(i, j)));
            if (scale[i] == T{}) throw std::domain_error("Singular matrix.");
        }
        const T threshold = std::numeric_limits<T>::epsilon() * static_cast<T>(rows);
        for (size_t i = 0; i < rows; ++i) {
            size_t pivot = i;
            for (size_t k = i + 1; k < rows; ++k)
                if (std::abs(A.at(k, i)) / scale[k] > std::abs(A.at(pivot, i)) / scale[pivot]) pivot = k;
            if (std::abs(A.at(pivot, i)) / scale[pivot] <= threshold)
                throw std::domain_error("Singular or numerically rank-deficient matrix.");
            for (size_t j = 0; j < cols; ++j) std::swap(A.at(i, j), A.at(pivot, j));
            for (size_t j = 0; j < b.cols; ++j) std::swap(x.at(i, j), x.at(pivot, j));
            std::swap(scale[i], scale[pivot]);
            for (size_t k = i + 1; k < rows; ++k) {
                const T factor = A.at(k, i) / A.at(i, i);
                A.at(k, i) = T{};
                for (size_t j = i + 1; j < cols; ++j) A.at(k, j) -= factor * A.at(i, j);
                for (size_t j = 0; j < b.cols; ++j) x.at(k, j) -= factor * x.at(i, j);
            }
        }
        for (size_t i = rows; i-- > 0;)
            for (size_t k = 0; k < b.cols; ++k) {
                for (size_t j = i + 1; j < cols; ++j) x.at(i, k) -= A.at(i, j) * x.at(j, k);
                x.at(i, k) /= A.at(i, i);
            }
        for (auto value : x.data)
            if (!std::isfinite(value)) throw std::runtime_error("Non-finite linear solution.");
        return x;
    }

    // Full-column-rank least squares using Householder QR (m >= n).
    Matrix least_squares(const Matrix& b) const {
        static_assert(std::is_floating_point_v<T>, "Least squares requires floating point.");
        if (!cols || rows < cols || b.rows != rows || !b.cols)
            throw std::invalid_argument("Least squares requires m >= n > 0 and matching RHS rows.");
        require_finite();
        b.require_finite();
        Matrix a(*this), rhs(b);
        std::vector<T> v(rows);
        const T threshold = std::numeric_limits<T>::epsilon() * static_cast<T>(rows);
        for (size_t k = 0; k < cols; ++k) {
            T original_norm = T{}, norm = T{};
            for (size_t i = 0; i < rows; ++i) original_norm = std::hypot(original_norm, at(i, k));
            for (size_t i = k; i < rows; ++i) norm = std::hypot(norm, a.at(i, k));
            if (!std::isfinite(norm) || !std::isfinite(original_norm))
                throw std::runtime_error("Least squares overflow.");
            if (original_norm == T{} || norm / original_norm <= threshold)
                throw std::domain_error("Rank-deficient least squares matrix.");
            const T sign = a.at(k, k) >= 0 ? T(1) : T(-1);
            for (size_t i = k; i < rows; ++i) v[i] = a.at(i, k) / norm;
            v[k] += sign;
            T vnorm = T{};
            for (size_t i = k; i < rows; ++i) vnorm = std::hypot(vnorm, v[i]);
            for (size_t i = k; i < rows; ++i) v[i] /= vnorm;
            for (size_t j = k; j < cols; ++j) {
                T dot = T{};
                for (size_t i = k; i < rows; ++i) dot += v[i] * a.at(i, j);
                for (size_t i = k; i < rows; ++i) a.at(i, j) -= T(2) * v[i] * dot;
            }
            a.at(k, k) = -sign * norm;
            for (size_t j = 0; j < b.cols; ++j) {
                T dot = T{};
                for (size_t i = k; i < rows; ++i) dot += v[i] * rhs.at(i, j);
                for (size_t i = k; i < rows; ++i) rhs.at(i, j) -= T(2) * v[i] * dot;
            }
        }
        Matrix result(cols, b.cols);
        for (size_t i = cols; i-- > 0;)
            for (size_t j = 0; j < b.cols; ++j) {
                T value = rhs.at(i, j);
                for (size_t k = i + 1; k < cols; ++k) value -= a.at(i, k) * result.at(k, j);
                result.at(i, j) = value / a.at(i, i);
                if (!std::isfinite(result.at(i, j))) throw std::runtime_error("Non-finite least squares solution.");
            }
        return result;
    }

    Matrix LinearFit(T* x_values, T* y_values, size_t n) {
        if (!x_values || !y_values || n < 2) {
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
    // Solve a linear system Ax = b using the Jacobi iterative method
    // Iteratively solves by decomposing A = D + R, then x_{k+1} = D^{-1}(b - R*x_k)
    // Converges when A is strictly diagonally dominant
    Matrix solve_jacobi(const Matrix& b, size_t maxIterations = 10000, double tolerance = 1e-10) const {
        static_assert(std::is_floating_point_v<T>, "Solving requires floating point.");
        if (!rows || rows != cols || b.rows != rows || b.cols != 1)
            throw std::invalid_argument("Invalid dimensions for solving linear system.");
        if (!maxIterations || !std::isfinite(tolerance) || tolerance <= 0)
            throw std::invalid_argument("Jacobi requires positive iterations and finite positive tolerance.");
        require_finite();
        b.require_finite();
        for (size_t i = 0; i < rows; ++i)
            if (at(i, i) == T{}) throw std::invalid_argument("Jacobi requires nonzero diagonal.");
        Matrix x(rows, 1), next(rows, 1);
        long double bnorm = 0;
        for (auto value : b.data) bnorm = std::hypot(bnorm, static_cast<long double>(value));
        for (size_t iter = 0; iter < maxIterations; ++iter) {
            for (size_t i = 0; i < rows; ++i) {
                T sigma = T{};
                for (size_t j = 0; j < cols; ++j)
                    if (j != i) sigma += at(i, j) * x.at(j, 0);
                next.at(i, 0) = (b.at(i, 0) - sigma) / at(i, i);
                if (!std::isfinite(next.at(i, 0))) throw std::runtime_error("Jacobi diverged.");
            }
            x.swap(next);
            long double residual = 0;
            for (size_t i = 0; i < rows; ++i) {
                long double value = -static_cast<long double>(b.at(i, 0));
                for (size_t j = 0; j < cols; ++j) value += static_cast<long double>(at(i, j)) * x.at(j, 0);
                residual = std::hypot(residual, value);
            }
            if (residual <= tolerance * std::max(1.0L, bnorm)) return x;
        }
        throw std::runtime_error("Jacobi did not converge within maxIterations.");
    }

    // Non-mutable power function using exponentiation by squaring
    Matrix<T> power(int exponent) const {
        if (exponent < 0) throw std::invalid_argument("Negative matrix powers are unsupported.");
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
            sum = std::hypot(sum, diff);
        }
        return sum;
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