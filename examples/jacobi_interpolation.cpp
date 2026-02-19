/**
 * Jacobi Iterative Method — Linear Interpolation Example
 * 
 * Demonstrates using the Jacobi iterative solver to perform a linear
 * (degree-1) least-squares fit through 4 data points.
 * 
 * We build the overdetermined Vandermonde system V * c = y, then form
 * the normal equations (V^T V) c = V^T y. For a degree-1 fit the
 * resulting 2x2 system is diagonally dominant, so Jacobi converges.
 */

#include <iostream>
#include <cmath>
#include <invariant/invariant.hpp>

using invariant::Matrix;

int main() {
    std::cout << "=== Jacobi Method: Linear Interpolation of 4 Points ===" << std::endl;
    std::cout << std::endl;

    // --- Define 4 data points ---
    const size_t n = 4;        // number of data points
    const size_t degree = 1;   // linear fit (y = c0 + c1*x)
    const size_t m = degree + 1;

    float x_vals[] = {1.0f, 2.0f, 4.0f, 5.0f};
    float y_vals[] = {2.0f, 3.0f, 5.5f, 7.0f};

    std::cout << "Data points:" << std::endl;
    for (size_t i = 0; i < n; ++i) {
        std::cout << "  (" << x_vals[i] << ", " << y_vals[i] << ")" << std::endl;
    }
    std::cout << std::endl;

    // --- Build the Vandermonde matrix V (n x m) and RHS y (n x 1) ---
    // For a degree-1 fit: each row is [1, x_i]
    Matrix<float> V(n, m);
    Matrix<float> y(n, 1);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            V.at(i, j) = std::pow(x_vals[i], static_cast<float>(j));
        }
        y.at(i, 0) = y_vals[i];
    }

    std::cout << "Vandermonde matrix V (" << n << "x" << m << "):" << std::endl;
    V.print();
    std::cout << std::endl;

    // --- Form the normal equations: (V^T V) c = V^T y ---
    Matrix<float> VtV = V.transpose() * V;
    Matrix<float> Vty = V.transpose() * y;

    std::cout << "Normal equations matrix (V^T * V):" << std::endl;
    VtV.print();
    std::cout << std::endl;

    std::cout << "Normal equations RHS (V^T * y):" << std::endl;
    Vty.print();
    std::cout << std::endl;

    // --- Verify diagonal dominance (required for Jacobi convergence) ---
    std::cout << "Diagonal dominance check:" << std::endl;
    for (size_t i = 0; i < m; ++i) {
        float diag = std::abs(VtV.at(i, i));
        float offDiagSum = 0.0f;
        for (size_t j = 0; j < m; ++j) {
            if (j != i) offDiagSum += std::abs(VtV.at(i, j));
        }
        std::cout << "  Row " << i << ": |a_ii| = " << diag
                  << ", sum|a_ij| = " << offDiagSum
                  << (diag > offDiagSum ? "  ✓ dominant" : "  ✗ NOT dominant")
                  << std::endl;
    }
    std::cout << std::endl;

    // --- Solve using Jacobi iterative method ---
    std::cout << "Solving with Jacobi iterative method..." << std::endl;
    Matrix<float> coeffs = VtV.solve_jacobi(Vty, 10000, 1e-6);

    // --- Display the fitted line ---
    float c0 = coeffs.at(0, 0);
    float c1 = coeffs.at(1, 0);
    std::cout << "\nFitted line: y = " << c0 << " + " << c1 << " * x" << std::endl;

    // --- Verify at original data points ---
    std::cout << "\nVerification at data points:" << std::endl;
    for (size_t i = 0; i < n; ++i) {
        float y_fit = c0 + c1 * x_vals[i];
        std::cout << "  x = " << x_vals[i]
                  << ":  y_data = " << y_vals[i]
                  << ",  y_fit = " << y_fit
                  << ",  residual = " << (y_vals[i] - y_fit) << std::endl;
    }

    // --- Compare with Gaussian elimination ---
    std::cout << "\n--- Comparison with Gaussian Elimination ---" << std::endl;
    Matrix<float> coeffs_gauss = VtV.solve(Vty);
    std::cout << "Gaussian elimination: y = " << coeffs_gauss.at(0, 0)
              << " + " << coeffs_gauss.at(1, 0) << " * x" << std::endl;
    std::cout << "Jacobi iteration:     y = " << c0
              << " + " << c1 << " * x" << std::endl;

    double diff = coeffs.two_norm_euclidian_length_difference(coeffs_gauss);
    std::cout << "Coefficient difference (2-norm): " << diff << std::endl;

    // --- Interpolate at intermediate points ---
    std::cout << "\n--- Interpolated values along the fitted line ---" << std::endl;
    for (float x = 0.0f; x <= 6.0f; x += 0.5f) {
        float y_interp = c0 + c1 * x;
        std::cout << "  p(" << x << ") = " << y_interp << std::endl;
    }

    return 0;
}
