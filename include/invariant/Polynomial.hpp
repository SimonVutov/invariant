#ifndef INVARIANT_POLYNOMIAL_HPP
#define INVARIANT_POLYNOMIAL_HPP

#include <climits>
#include <invariant/Matrix.hpp>

namespace invariant {

template <typename T> T pow_O_ln_n_rec(T x, int n);

template <typename T>
T polyval_O_n_ln_n_rec(const T coeffs[], unsigned int degree, T x) {
    if (!coeffs) throw std::invalid_argument("Null polynomial coefficients.");
    T result{ coeffs[0]};
    for (unsigned int k{1}; k <= degree; k++) {
        result += coeffs[k] * pow_O_ln_n_rec(x, k);
    }
    return result;
}

template <typename T>
T pow_O_ln_n_iter(T x, int n) {
    if (n < 0 && (x == T{} || !std::is_floating_point_v<T>))
        throw std::domain_error("Negative powers require nonzero floating-point base.");
    if (n >= 0) {
        T result = T(1);
        while (n > 0) {
            if ((n & 1) == 1) result *= x;
            x *= x;
            n >>= 1;
        }
        return result;
    }
    else if (n == INT_MIN) {
        return pow_O_ln_n_iter(T(1)/x, -(n+1))/x;
    } else {
        return pow_O_ln_n_iter( T(1)/x, -n);
    }
}

template <typename T>
T polyval_horner (const T coeffs[], unsigned int degree, T x) {
    if (!coeffs) throw std::invalid_argument("Null polynomial coefficients.");
    T result{coeffs[degree]};
    for (unsigned int k{degree - 1}; k <= degree; k--) {
        result = result * x + coeffs[k];
    }
    return result;
}
template <typename T> // 1% faster version with pointers
T polyval_horner2 (const T coeffs[], unsigned int degree, T x) {
    if (!coeffs) throw std::invalid_argument("Null polynomial coefficients.");
    const T *coeff{coeffs + degree};
    T result{*coeff};
    while (coeff > coeffs) {
        result = x*result + *--coeff;
    }
    return result;
}

// A recursive means of calculating x^n
template <typename T>
T pow_O_ln_n_rec(T x, int n) {
    if (n < 0 && (x == T{} || !std::is_floating_point_v<T>))
        throw std::domain_error("Negative powers require nonzero floating-point base.");
    if (n > 0) {
        T result{pow_O_ln_n_rec(x, n/2)};
        result *= result;
        return ((n&1) == 0) ? result : result*x;
    } else if (n == 0) {
        return 1.0;
    } else if (n == INT_MIN) {
        return pow_O_ln_n_rec( T(1)/x, -(n+1)) / x;
    } else {
        return pow_O_ln_n_rec( T(1)/x, -n);
    }
}

inline float eval(const Matrix<float>& m, float x) { // polynomial horner's method
    if (m.getRows() != 1) throw std::invalid_argument("Polynomial must be a row matrix.");
    float res = 0.0f;
    for (size_t i = m.getCols(); i-- > 0;) {
        res = res * x + m.at(0, i);
    }
    return res;
}

}

#endif  // INVARIANT_POLYNOMIAL_HPP