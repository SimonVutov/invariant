/**
 * Polynomial Fitting Example
 * 
 * Demonstrates polynomial interpolation and evaluation using the
 * LinearFit method and Horner's method for polynomial evaluation.
 */

#include <iostream>
#include <invariant/invariant.hpp>

using invariant::Matrix;
using invariant::polyval_horner;
using invariant::pow_O_ln_n_iter;

void polynomialFittingExample();
void polynomialEvaluationExample();

int main() {
    std::cout << "=== Polynomial Operations Examples ===" << std::endl << std::endl;
    
    polynomialFittingExample();
    std::cout << std::endl;
    
    polynomialEvaluationExample();
    
    return 0;
}

/**
 * Example 1: Polynomial Interpolation
 * Fits a polynomial through given data points
 */
void polynomialFittingExample() {
    std::cout << "--- Example 1: Polynomial Fitting ---" << std::endl;
    
    // Data points to fit
    float x_vals[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float y_vals[] = {2.0f, 3.0f, 5.0f, 10.0f, 15.0f};
    
    std::cout << "Data points:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        std::cout << "  (" << x_vals[i] << ", " << y_vals[i] << ")" << std::endl;
    }
    
    // Fit polynomial through the points
    Matrix<float> mat;
    std::cout << "\nFitting polynomial (degree 4) through points:" << std::endl;
    std::cout << "Equation: ";
    Matrix<float> coefficients = mat.LinearFit(x_vals, y_vals, 5);
    
    std::cout << "\nCoefficient matrix:" << std::endl;
    coefficients.print();
    
    // Verify fit by evaluating at original points
    std::cout << "\nVerification (evaluating at original x values):" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        float x = x_vals[i];
        float y_actual = y_vals[i];
        float y_fitted = 0.0f;
        
        // Evaluate polynomial using Horner's method
        for (size_t j = 0; j < coefficients.getRows(); ++j) {
            y_fitted += coefficients.at(j, 0) * pow_O_ln_n_iter(x, static_cast<int>(j));
        }
        
        std::cout << "  x=" << x << ": actual=" << y_actual 
                  << ", fitted=" << y_fitted 
                  << ", error=" << std::abs(y_actual - y_fitted) << std::endl;
    }
}

/**
 * Example 2: Efficient Polynomial Evaluation
 * Demonstrates Horner's method for polynomial evaluation
 */
void polynomialEvaluationExample() {
    std::cout << "--- Example 2: Polynomial Evaluation (Horner's Method) ---" << std::endl;
    
    // Polynomial: 2 + 3x + 4x^2 + 5x^3
    float coeffs[] = {2.0f, 3.0f, 4.0f, 5.0f};
    unsigned int degree = 3;
    
    std::cout << "Polynomial: 2 + 3x + 4x^2 + 5x^3" << std::endl;
    std::cout << "\nEvaluating at different values:" << std::endl;
    
    for (float x = 0.0f; x <= 3.0f; x += 0.5f) {
        float result = polyval_horner(coeffs, degree, x);
        std::cout << "  p(" << x << ") = " << result << std::endl;
    }
    
    // Compare with direct evaluation
    std::cout << "\nEfficiency Note:" << std::endl;
    std::cout << "Horner's method requires only n multiplications and n additions" << std::endl;
    std::cout << "compared to naive evaluation which requires O(n²) operations." << std::endl;
}