/**
 * Sample Usage Examples for the Invariant Library
 * 
 * This file demonstrates the core functionality of the Matrix class
 * including construction, operations, and numerical methods.
 */

#include <iostream>
#include <invariant/invariant.hpp>

using invariant::Matrix;

// Forward declarations
void basicMatrixOperations();
void matrixMultiplication();
void solveLinearSystem();
void stochasticMatrixAnalysis();
void matrixPowerAndConvergence();

int main() {
    std::cout << "=== Invariant Library Examples ===" << std::endl << std::endl;
    
    // Run all examples
    basicMatrixOperations();
    std::cout << std::endl;
    
    matrixMultiplication();
    std::cout << std::endl;
    
    solveLinearSystem();
    std::cout << std::endl;
    
    stochasticMatrixAnalysis();
    std::cout << std::endl;
    
    matrixPowerAndConvergence();
    
    return 0;
}

/**
 * Example 1: Basic Matrix Construction and Operations
 * Demonstrates creating matrices and basic element access
 */
void basicMatrixOperations() {
    std::cout << "--- Example 1: Basic Matrix Operations ---" << std::endl;
    
    // Create a 3x3 matrix initialized with value 2.0
    Matrix<double> A(3, 3, 2.0);
    std::cout << "Matrix A (3x3, filled with 2.0):" << std::endl;
    A.print();
    
    // Create a 2x3 matrix with random values between 0 and 10
    Matrix<double> B(2, 3, 0.0, 10.0);
    std::cout << "\nMatrix B (2x3, random values [0, 10]):" << std::endl;
    B.print();
    
    // Element access and modification
    Matrix<double> C(2, 2, 0.0);
    C.at(0, 0) = 1.0;
    C.at(0, 1) = 2.0;
    C.at(1, 0) = 3.0;
    C.at(1, 1) = 4.0;
    std::cout << "\nMatrix C (2x2, manually set):" << std::endl;
    C.print();
    
    // Transpose
    Matrix<double> C_transpose = C.transpose();
    std::cout << "\nC transpose:" << std::endl;
    C_transpose.print();
    
    // Matrix dimensions
    std::cout << "\nDimensions: " << C.dim() << std::endl;
}

/**
 * Example 2: Matrix Multiplication
 * Shows matrix-matrix and matrix-scalar multiplication
 */
void matrixMultiplication() {
    std::cout << "--- Example 2: Matrix Multiplication ---" << std::endl;
    
    // Create two compatible matrices
    Matrix<double> A(2, 3, 1.0);
    Matrix<double> B(3, 2, 2.0);
    
    std::cout << "Matrix A (2x3):" << std::endl;
    A.print();
    std::cout << "\nMatrix B (3x2):" << std::endl;
    B.print();
    
    // Matrix multiplication
    Matrix<double> C = A * B;
    std::cout << "\nA * B (2x2):" << std::endl;
    C.print();
    
    // Scalar multiplication
    Matrix<double> D = A * 3.0;
    std::cout << "\nA * 3.0:" << std::endl;
    D.print();
}

/**
 * Example 3: Solving Linear Systems
 * Demonstrates Ax = b using Gaussian elimination
 */
void solveLinearSystem() {
    std::cout << "--- Example 3: Solving Linear System Ax = b ---" << std::endl;
    
    // Create system: 2x + y = 11, 5x + 7y = 13
    double** data = new double*[2];
    data[0] = new double[2]{2, 1};
    data[1] = new double[2]{5, 7};
    Matrix<double> A(2, 2, data);
    
    Matrix<double> b(2, 1);
    b.at(0, 0) = 11;
    b.at(1, 0) = 13;
    
    std::cout << "System matrix A:" << std::endl;
    A.print();
    std::cout << "\nRight-hand side b:" << std::endl;
    b.print();
    
    // Solve the system
    Matrix<double> x = A.solve(b);
    std::cout << "\nSolution x:" << std::endl;
    x.print();
    
    // Verify: A * x should equal b
    Matrix<double> verification = A * x;
    std::cout << "\nVerification (A * x):" << std::endl;
    verification.print();
    
    // Cleanup
    delete[] data[0];
    delete[] data[1];
    delete[] data;
}

/**
 * Example 4: Stochastic Matrices
 * Demonstrates column-stochastic matrices (columns sum to 1)
 */
void stochasticMatrixAnalysis() {
    std::cout << "--- Example 4: Stochastic Matrix Analysis ---" << std::endl;
    
    // Create a random matrix
    Matrix<double> A(3, 3, 1.0, 5.0);
    std::cout << "Original matrix:" << std::endl;
    A.print();
    
    // Make it column-stochastic
    A.makeColumnStochastic();
    std::cout << "\nColumn-stochastic matrix (columns sum to 1):" << std::endl;
    A.print();
    
    // Verify column sums
    std::cout << "\nVerifying column sums:" << std::endl;
    for (size_t j = 0; j < A.getCols(); ++j) {
        double colSum = 0.0;
        for (size_t i = 0; i < A.getRows(); ++i) {
            colSum += A.at(i, j);
        }
        std::cout << "  Column " << j << " sum: " << colSum << std::endl;
    }
}

/**
 * Example 5: Matrix Powers and Convergence
 * Shows efficient matrix exponentiation and steady-state analysis
 */
void matrixPowerAndConvergence() {
    std::cout << "--- Example 5: Matrix Powers and Steady State ---" << std::endl;
    
    // Create a 2x2 stochastic matrix (Markov chain)
    double** data = new double*[2];
    data[0] = new double[2]{0.7, 0.3};  // Stay/transition probabilities
    data[1] = new double[2]{0.3, 0.7};
    Matrix<double> P(2, 2, data);
    
    std::cout << "Transition matrix P:" << std::endl;
    P.print();
    
    // Compute high powers to find steady state
    std::cout << "\nConvergence to steady state:" << std::endl;
    Matrix<double> prev = P;
    for (int k : {5, 10, 20, 50}) {
        Matrix<double> Pk = P.power(k);
        std::cout << "\nP^" << k << ":" << std::endl;
        Pk.print();
        
        if (k > 5) {
            double diff = Pk.two_norm_euclidian_length_difference(prev);
            std::cout << "  Change from previous: " << diff << std::endl;
        }
        prev = Pk;
    }
    
    // Apply to initial state vector
    Matrix<double> initial(2, 1);
    initial.at(0, 0) = 1.0;  // Start in state 1
    initial.at(1, 0) = 0.0;
    
    std::cout << "\nInitial state:" << std::endl;
    initial.print();
    
    Matrix<double> final_state = P.power(50) * initial;
    std::cout << "\nState after 50 steps:" << std::endl;
    final_state.print();
    std::cout << "Sum: " << final_state.sum() << std::endl;
    
    // Cleanup
    delete[] data[0];
    delete[] data[1];
    delete[] data;
}