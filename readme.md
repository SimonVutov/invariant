# Invariant

Invariant is a header-only C++17 library providing template-based linear algebra and numerical method implementations.

This is a Matrix/Vector/Polynomial Library that incorporates concepts of Linear Algebra (uWaterloo ECE115), Numerical Methods (uWaterloo ECE204 by Douglas Wilhelm Harder). A lot of the code comes directly from Harder's lessons.

## Objectives

This repository aims to:

- Implement numerical methods and algorithms from Computer Engineering courses (ECE115, ECE204)
- Demonstrate C++ library development practices
- [Future] Integration with Python through pybind11 bindings

## Project Structure

```
invariant/
├── CMakeLists.txt              # Build configuration
├── LICENSE                     # MIT License
├── README.md
├── include/
│   └── invariant/
│       ├── Matrix.hpp          # Matrix class (header-only)
│       ├── Polynomial.hpp      # Polynomial functions (header-only)
│       └── invariant.hpp       # Convenience header (includes all)
├── examples/
│   ├── CMakeLists.txt
│   ├── sample_usage.cpp              # Matrix operations demo
│   ├── linear_fit.cpp                # Polynomial fitting demo
│   └── jacobi_interpolation.cpp      # Jacobi solver & linear interpolation demo
└── tests/
    ├── CMakeLists.txt
    └── test_matrix.cpp         # Unit tests
```

## Installation

### Method 1: System Install (Recommended)

```bash
git clone https://github.com/simonvutov/invariant.git
cd invariant
mkdir build && cd build
cmake ..
sudo make install
```

### Method 2: CMake Subdirectory

Add to your project:

```bash
git submodule add https://github.com/simonvutov/invariant external/invariant
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory(external/invariant)
target_link_libraries(your_app PRIVATE invariant)
```

### Method 3: Header-Only Copy

Copy the `include/invariant/` directory to your project.

## Usage

```cpp
#include <invariant/Matrix.hpp>
// or
#include <invariant/invariant.hpp>  // Everything

using invariant::Matrix;

int main() {
    // Create a 3x3 matrix filled with 1.0
    Matrix<double> m(3, 3, 1.0);
    m.print();

    // Matrix multiplication
    Matrix<double> result = m * m;
    result.print();

    return 0;
}
```

### Compiling Your Code

```bash
g++ -std=c++17 -I/usr/local/include your_code.cpp -o your_app
```

Or with CMake:

```cmake
find_package(invariant REQUIRED)
target_link_libraries(your_app PRIVATE invariant::invariant)
```

## Examples

The library includes examples:

- **`examples/sample_usage`** - Matrix operations:
    - Matrix construction and element access
    - Matrix multiplication (matrix-matrix and matrix-scalar)
    - Solving linear systems with Gaussian elimination
    - Stochastic matrices and column normalization
    - Matrix powers and convergence analysis

- **`examples/linear_fit`** - Polynomial operations:
    - Polynomial interpolation through data points
    - Polynomial evaluation with Horner's method
    - Coefficient extraction and verification

- **`examples/jacobi_interpolation`** - Jacobi iterative solver:
    - Solving linear systems using the Jacobi iterative method
    - Least-squares linear interpolation of 4 data points
    - Building Vandermonde matrices and normal equations
    - Diagonal dominance verification
    - Comparison of Jacobi iteration vs Gaussian elimination

### Running the Examples

First, build the project from scratch:

```bash
git clone https://github.com/simonvutov/invariant.git
cd invariant
mkdir build && cd build
cmake ..
make
```

Then run any example:

```bash
./examples/sample_usage
./examples/linear_fit
./examples/jacobi_interpolation
```

## Testing

Run the test suite to verify the library:

```bash
cd build
./tests/test_matrix
# or use CTest
ctest
```

Tests cover:

- Matrix construction (default, sized, with values)
- Matrix operations (addition, subtraction, transpose)
- Matrix multiplication
- Column-stochastic transformations
- Linear system solving
- Matrix exponentiation

## Development Notes

### Build & Test

```bash
# Clean build from scratch
rm -rf build && mkdir build && cd build

# Configure with CMake
cmake ..

# Build all targets (examples and tests)
make

# Run examples
./examples/sample_usage
./examples/linear_fit

# Run tests
./tests/test_matrix
# or use CTest
ctest
```

### Installation

```bash
# From build directory
sudo make install
```

This installs headers to `/usr/local/include/invariant/` so others can use:

```cpp
#include <invariant/Matrix.hpp>
```
