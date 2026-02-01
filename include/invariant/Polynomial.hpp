#ifndef INVARIANT_POLYNOMIAL_HPP
#define INVARIANT_POLYNOMIAL_HPP

#include <climits>
#include <invariant/Matrix.hpp>

namespace invariant {

template <typename T>
T polyval_O_n_ln_n_rec(T coeffs[], unsigned int degree, T x) {
    T result{ coeffs[0]};
    for (unsigned int k{1}; k <= degree; k++) {
        result += coeffs[k] * pow_O_ln_n_rec(x, k);
    }
    return result;
}

template <typename T>
T pow_O_ln_n_iter(T x, int n) {
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
        return pow_O_ln_n_iter(1.0/x, -(n+1))/x;
    } else {
        return pow_O_ln_n_iter( 1.0/x, -n);
    }
}

template <typename T>
T polyval_horner (T coeffs[], unsigned int degree, T x) {
    T result{coeffs[degree]};
    for (unsigned int k{degree - 1}; k <= degree; k--) {
        result = result * x + coeffs[k];
    }
    return result;
}
template <typename T> // 1% faster version with pointers
T polyval_horner2 (T coeffs[], unsigned int degree, T x) {
    T *coeff{coeffs + degree};
    T result{*coeff};
    while (coeff > coeffs) {
        result = x*result + *--coeff;
    }
    return result;
}

// A recursive means of calculating x^n
template <typename T>
T pow_O_ln_n_rec(T x, int n) {
    if (n > 0) {
        T result{pow_O_ln_n_rec(x, n/2)};
        result *= result;
        return ((n&1) == 0) ? result : result*x;
    } else if (n == 0) {
        return 1.0;
    } else if (n == INT_MIN) {
        return pow_O_ln_n_rec( 1.0/x, -(n+1)) / x;
    } else {
        return pow_O_ln_n_rec( 1.0/x, -n);
    }
}

float eval(Matrix<float>& m, float x) { // polynomial horner's method
    float res = 0.0f;
    for (int i = m.getCols() - 1; i >= 0; i--) {
        res = res * x + m.at(0, i);
    }
    return res;
}

}

#endif  // INVARIANT_POLYNOMIAL_HPP