/**
 * Unit Tests for Invariant Library
 * 
 * Tests core functionality of the Matrix class to ensure correctness.
 * Run this to verify the library works as expected.
 */

#include <iostream>
#include <cmath>
#include <cassert>
#include <invariant/invariant.hpp>

using invariant::Matrix;

// Test function declarations
void testMatrixConstruction();
void testMatrixOperations();
void testMatrixMultiplication();
void testMatrixTranspose();
void testColumnStochastic();
void testLinearSystemSolver();
void testMatrixPower();

int main() {
    std::cout << "=== Running Invariant Library Tests ===" << std::endl << std::endl;
    
    int tests_passed = 0;
    int tests_failed = 0;
    
    try {
        testMatrixConstruction();
        std::cout << "✓ Matrix construction tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Matrix construction tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testMatrixOperations();
        std::cout << "✓ Matrix operations tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Matrix operations tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testMatrixMultiplication();
        std::cout << "✓ Matrix multiplication tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Matrix multiplication tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testMatrixTranspose();
        std::cout << "✓ Matrix transpose tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Matrix transpose tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testColumnStochastic();
        std::cout << "✓ Column stochastic tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Column stochastic tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testLinearSystemSolver();
        std::cout << "✓ Linear system solver tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Linear system solver tests FAILED" << std::endl;
        tests_failed++;
    }
    
    try {
        testMatrixPower();
        std::cout << "✓ Matrix power tests passed" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "✗ Matrix power tests FAILED" << std::endl;
        tests_failed++;
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;
    
    return tests_failed > 0 ? 1 : 0;
}

/**
 * Test 1: Matrix Construction
 */
void testMatrixConstruction() {
    // Test default constructor
    Matrix<double> m1;
    assert(m1.getRows() == 0);
    assert(m1.getCols() == 0);
    
    // Test size constructor
    Matrix<double> m2(3, 4);
    assert(m2.getRows() == 3);
    assert(m2.getCols() == 4);
    
    // Test constructor with initial value
    Matrix<double> m3(2, 2, 5.0);
    assert(m3.at(0, 0) == 5.0);
    assert(m3.at(1, 1) == 5.0);
    
    // Test copy constructor
    Matrix<double> m4(m3);
    assert(m4.at(0, 0) == 5.0);
    assert(m4.getRows() == 2);
}

/**
 * Test 2: Matrix Operations (Add, Subtract)
 */
void testMatrixOperations() {
    Matrix<double> A(2, 2, 3.0);
    Matrix<double> B(2, 2, 2.0);
    
    // Test addition
    Matrix<double> C = A.Add(B);
    assert(std::abs(C.at(0, 0) - 5.0) < 1e-10);
    assert(std::abs(C.at(1, 1) - 5.0) < 1e-10);
    
    // Test subtraction
    Matrix<double> D = A.Subtract(B);
    assert(std::abs(D.at(0, 0) - 1.0) < 1e-10);
    assert(std::abs(D.at(1, 1) - 1.0) < 1e-10);
}

/**
 * Test 3: Matrix Multiplication
 */
void testMatrixMultiplication() {
    // Create 2x2 matrices for testing
    double** data_a = new double*[2];
    data_a[0] = new double[2]{1, 2};
    data_a[1] = new double[2]{3, 4};
    Matrix<double> A(2, 2, data_a);
    
    double** data_b = new double*[2];
    data_b[0] = new double[2]{2, 0};
    data_b[1] = new double[2]{1, 2};
    Matrix<double> B(2, 2, data_b);
    
    // A * B = [[4, 4], [10, 8]]
    Matrix<double> C = A * B;
    assert(std::abs(C.at(0, 0) - 4.0) < 1e-10);
    assert(std::abs(C.at(0, 1) - 4.0) < 1e-10);
    assert(std::abs(C.at(1, 0) - 10.0) < 1e-10);
    assert(std::abs(C.at(1, 1) - 8.0) < 1e-10);
    
    // Scalar multiplication
    Matrix<double> D = A * 2.0;
    assert(std::abs(D.at(0, 0) - 2.0) < 1e-10);
    assert(std::abs(D.at(1, 1) - 8.0) < 1e-10);
    
    // Cleanup
    delete[] data_a[0]; delete[] data_a[1]; delete[] data_a;
    delete[] data_b[0]; delete[] data_b[1]; delete[] data_b;
}

/**
 * Test 4: Matrix Transpose
 */
void testMatrixTranspose() {
    double** data = new double*[2];
    data[0] = new double[3]{1, 2, 3};
    data[1] = new double[3]{4, 5, 6};
    Matrix<double> A(2, 3, data);
    
    Matrix<double> At = A.transpose();
    
    // Check dimensions swapped
    assert(At.getRows() == 3);
    assert(At.getCols() == 2);
    
    // Check values transposed
    assert(std::abs(At.at(0, 0) - 1.0) < 1e-10);
    assert(std::abs(At.at(1, 0) - 2.0) < 1e-10);
    assert(std::abs(At.at(2, 1) - 6.0) < 1e-10);
    
    // Cleanup
    delete[] data[0]; delete[] data[1]; delete[] data;
}

/**
 * Test 5: Column Stochastic Transformation
 */
void testColumnStochastic() {
    double** data = new double*[2];
    data[0] = new double[3]{1, 2, 3};
    data[1] = new double[3]{4, 5, 6};
    Matrix<double> A(2, 3, data);
    
    A.makeColumnStochastic();
    
    // Check each column sums to 1
    for (size_t j = 0; j < A.getCols(); ++j) {
        double sum = 0.0;
        for (size_t i = 0; i < A.getRows(); ++i) {
            sum += A.at(i, j);
        }
        assert(std::abs(sum - 1.0) < 1e-10);
    }
    
    // Check specific values: column 0 should be [1/5, 4/5]
    assert(std::abs(A.at(0, 0) - 0.2) < 1e-10);
    assert(std::abs(A.at(1, 0) - 0.8) < 1e-10);
    
    // Cleanup
    delete[] data[0]; delete[] data[1]; delete[] data;
}

/**
 * Test 6: Linear System Solver
 */
void testLinearSystemSolver() {
    // System: 2x + y = 11, 5x + 7y = 13
    // Solution: x ≈ 7.111, y ≈ -3.222
    double** data = new double*[2];
    data[0] = new double[2]{2, 1};
    data[1] = new double[2]{5, 7};
    Matrix<double> A(2, 2, data);
    
    Matrix<double> b(2, 1);
    b.at(0, 0) = 11;
    b.at(1, 0) = 13;
    
    Matrix<double> x = A.solve(b);
    
    // Check solution
    assert(std::abs(x.at(0, 0) - 7.111111) < 1e-5);
    assert(std::abs(x.at(1, 0) - (-3.222222)) < 1e-5);
    
    // Verify: A*x should equal b
    Matrix<double> verification = A * x;
    assert(std::abs(verification.at(0, 0) - 11.0) < 1e-5);
    assert(std::abs(verification.at(1, 0) - 13.0) < 1e-5);
    
    // Cleanup
    delete[] data[0]; delete[] data[1]; delete[] data;
}

/**
 * Test 7: Matrix Power (Exponentiation by Squaring)
 */
void testMatrixPower() {
    // Test A^2 where A = [[1,2],[3,4]]
    // A^2 = [[7, 10], [15, 22]]
    double** data = new double*[2];
    data[0] = new double[2]{1, 2};
    data[1] = new double[2]{3, 4};
    Matrix<double> A(2, 2, data);
    
    Matrix<double> A2 = A.power(2);
    
    assert(std::abs(A2.at(0, 0) - 7.0) < 1e-10);
    assert(std::abs(A2.at(0, 1) - 10.0) < 1e-10);
    assert(std::abs(A2.at(1, 0) - 15.0) < 1e-10);
    assert(std::abs(A2.at(1, 1) - 22.0) < 1e-10);
    
    // Test A^0 = Identity
    Matrix<double> A0 = A.power(0);
    assert(std::abs(A0.at(0, 0) - 1.0) < 1e-10);
    assert(std::abs(A0.at(1, 1) - 1.0) < 1e-10);
    assert(std::abs(A0.at(0, 1)) < 1e-10);
    assert(std::abs(A0.at(1, 0)) < 1e-10);
    
    // Cleanup
    delete[] data[0]; delete[] data[1]; delete[] data;
}
