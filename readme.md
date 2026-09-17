# Invariant

Invariant is a dependency-free, header-only C++17 library for dense matrices and
polynomial evaluation. The source is versioned **0.1.0**; a published release/tag
is a separate step. It is intended for small numerical programs and learning,
with explicit errors for invalid operations.

The project began with linear algebra and numerical methods from Waterloo
ECE115 and ECE204. Original implementations were adapted from Douglas Wilhelm
Harder's lessons. See [LICENSE](LICENSE).

## Build and test

Requires a C++17 compiler and CMake 3.20 or newer. Header-only consumers do not
need CMake or any external package.

```sh
git clone https://github.com/SimonVutov/invariant.git
cd invariant
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

Tests remain active in Release builds. They cover matrix operations, ownership,
rectangular and empty multiplication, invalid inputs, singular and scaled
systems, generated systems with known solutions, Jacobi convergence and failure,
polynomials, linking multiple translation units, all examples, and an installed
consumer. The installation test moves the installed package before consuming it.

The CI workflow defines Debug/Release checks on Linux, macOS, and Windows, plus
AddressSanitizer/UndefinedBehaviorSanitizer on Linux. Local verification results
are in [CHANGELOG.md](CHANGELOG.md); configured CI is not a claim that remote jobs
have already run.

For a local sanitizer build with Clang or GCC:

```sh
cmake -S . -B build/sanitized -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build/sanitized --parallel
ctest --test-dir build/sanitized --output-on-failure
```

## Use from C++

```cpp
#include <invariant/invariant.hpp>
#include <iostream>

int main() {
    invariant::Matrix<double> a(2, 2), b(2, 1);
    a.at(0, 0) = 4; a.at(0, 1) = 1;
    a.at(1, 0) = 1; a.at(1, 1) = 3;
    b.at(0, 0) = 1; b.at(1, 0) = 2;
    const auto x = a.solve(b);
    x.print(); // approximately 0.0909091, 0.636364
    std::cout << (a * x).two_norm_euclidian_length_difference(b) << '\n';
}
```

Compile directly with `c++ -std=c++17 -Iinclude your_program.cpp -o your_program`.

### CMake dependency

Use a checkout as a subdirectory:

```cmake
add_subdirectory(external/invariant)
add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE invariant::invariant)
```

Or install to a user-owned prefix:

```sh
cmake --install build --config Release --prefix "$HOME/.local"
```

Then in the consuming project:

```cmake
find_package(invariant 0.1 CONFIG REQUIRED)
add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE invariant::invariant)
```

Configure that project with `-DCMAKE_PREFIX_PATH="$HOME/.local"`. The package
exports its include directories and C++17 requirement. You can also copy the
`include/invariant` directory directly.

Build options are `BUILD_TESTS`, `BUILD_EXAMPLES`, and `BUILD_BENCHMARKS`.
Tests/examples default on for standalone builds and off for subdirectory use;
benchmarks default off. The original option names and unnamespaced `invariant`
target remain available for compatibility.

## API and numerical behavior

Matrices own contiguous row-major storage. Copies are independent; moves leave
the source as a valid 0×0 matrix. `at(row, col)` checks both dimensions. Zero-sized
matrices are valid for storage, transpose, and multiplication; solvers require
nonempty square matrices. The pointer constructor copies its input; callers must
provide the declared number of readable rows and elements.

| Operation | Interface and behavior |
| --- | --- |
| Dimensions | `getRows()`, `getCols()`, `dim()` |
| Arithmetic | `a * b`, `a * scalar`, `a.Add(b)`, `a.Subtract(b)`, `transpose()` |
| Elementwise reciprocal | `reciprocal()`; rejects zero entries; integer matrices retain integer division semantics |
| Linear solve | `a.solve(b)`; scaled partial pivoting, one or more RHS columns |
| Iterative solve | `a.solve_jacobi(b, maxIterations=10000, tolerance=1e-10)`; one RHS column |
| Powers | `a.power(nonnegative_integer)`; exponent zero produces the identity |
| Column normalization | `makeColumnStochastic()`; requires nonnegative finite entries and positive finite column sums |
| Difference norm | `two_norm_euclidian_length_difference(b)`; Frobenius norm, with overflow-resistant accumulation |
| Reduction | `sum()` returns `double` |
| Interpolation | Legacy `LinearFit(x, y, n)` returns ascending coefficients of a degree-at-most `n-1` interpolating polynomial and prints it |
| Polynomial evaluation | `polyval_horner(coeffs, degree, x)`, `polyval_horner2`, `polyval_O_n_ln_n_rec`; ascending coefficients |
| Scalar powers | `pow_O_ln_n_iter(x, n)`, `pow_O_ln_n_rec(x, n)` |

`solve`, `solve_jacobi`, and stochastic normalization require floating-point
matrices at compile time. Use `Matrix<float>` or `Matrix<double>`. Integer
arithmetic otherwise follows C++ rules, including overflow limitations.

- Bad shapes, non-finite solver inputs, invalid iteration controls, negative
  matrix powers, and invalid normalization inputs throw `std::invalid_argument`.
- Invalid indices throw `std::out_of_range`; impossible allocation dimensions
  throw `std::length_error`. Ordinary allocation failure can throw `std::bad_alloc`.
- `solve` throws `std::domain_error` for a zero row or a pivot whose magnitude,
  divided by its original row scale, is at most `epsilon(T) * rows`. This is a
  numerical rank test, not a condition-number estimate. Ill-conditioned systems
  may still yield inaccurate answers: inspect the residual and input scaling.
- Jacobi checks `||A*x-b||₂ <= tolerance * max(1, ||b||₂)`. Strict diagonal
  dominance is a useful sufficient convergence condition. It throws
  `std::runtime_error` on non-finite iterates or exhausted iterations; it never
  silently returns an unconverged result. A non-finite direct solution also
  throws `std::runtime_error`.
- Column normalization validates every column before changing any entries.
- Negative scalar powers require a nonzero floating-point base. Negative powers
  of integer bases throw `std::domain_error` to avoid silent truncation.

Polynomial pointer APIs require at least `degree + 1` readable coefficients;
null pointers are rejected. `eval` is a legacy helper for a one-row
`Matrix<float>`. `LinearFit` is polynomial interpolation, despite its historical
name; it is not a general least-squares fitting API.

## Examples and benchmarks

The build provides four examples:

```sh
./build/examples/sample_usage
./build/examples/linear_fit
./build/examples/jacobi_interpolation
./build/examples/checked_solve
```

For multi-configuration generators, executables are under the corresponding
`Release` or `Debug` subdirectory. `checked_solve` demonstrates a residual check
and handling a singular-system exception.

```sh
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
cmake --build build/release --config Release --parallel
./build/release/matrix_benchmark
```

The benchmark reports CSV for square multiplication, a dense-batch multiplication
shape, and a multi-RHS solve. See [methodology and measured results](docs/benchmarks.md).
These are single-threaded CPU microbenchmarks, not neural-network training results
or comparisons against optimized BLAS implementations.

## Python and CPPNN

Python bindings and neural-network training are not part of this release-hardening
pass. Invariant currently has no Python package. Neural-network layers and
CIFAR-10/MNIST workflows belong in the existing
[CPPNN project](https://github.com/SimonVutov/CPPNN).

CPPNN can consume Invariant as a numerical dependency; neural-network layers stay in CPPNN.
