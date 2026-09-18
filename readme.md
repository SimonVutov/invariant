# Invariant

Header-only C++17 dense matrices and numerical methods, with NumPy-friendly
Python bindings. Version **0.1.0**. No BLAS or neural-network framework required.

## Get the code

```sh
git clone https://github.com/SimonVutov/invariant.git
cd invariant
```

Until the release work is merged, use `git checkout codex/release-0.1-hardening`.
The commands below run from this repository.

## Python

Requires Python 3.10+, a C++17 compiler, and CMake 3.20+ for source builds.

```sh
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
python -m pip install '.[test]'
python -m pytest
python examples/python/matrix_basics.py
python examples/python/linear_regression.py
python examples/python/benchmark.py --size 128
```

The distribution is named `invariant-numerics`; import it as `invariant`.
Installation above uses this checkout; no PyPI publication is assumed.

```python
import invariant as iv

x = iv.solve([[4., 1.], [1., 3.]], [1., 2.])
coefficients = iv.least_squares([[1., 0.], [1., 1.], [1., 2.]], [1., 3., 5.])
a = iv.Matrix([[1., 2.], [3., 4.]])
print(x, coefficients, (a @ a).numpy())
```

`matmul`, `solve`, `least_squares`, and `solve_jacobi` execute C++ kernels.
Array functions return float32 only when both inputs are float32, otherwise
float64; complex/object arrays are rejected. Solvers accept a 1D RHS and preserve
its dimensionality. Matrix multiplication requires two 2D arrays.

`Matrix` stores float64; `MatrixFloat` stores float32. They support `[row, col]`
(including negative indices), `@`, `+`, `-`, scalar `*`, `.T`, `.solve(b)`,
`.least_squares(b)`, `.solve_jacobi(b)`, `.power(n)`, `.sum()`, `.reciprocal()`,
`.difference_norm(b)`, and `.make_column_stochastic()`.
Construction and `.numpy()` copy: NumPy mutations never alias C++ storage.
Array functions release the GIL during computation on their owned copies.
`polyval([a, b, c], x)` evaluates `a + b*x + c*x*x`.

## C++

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix "$HOME/.local"
```

```cpp
#include <invariant/invariant.hpp>
int main() {
    invariant::Matrix<double> a(2, 2), b(2, 1);
    a.at(0, 0)=4; a.at(0, 1)=1; a.at(1, 0)=1; a.at(1, 1)=3;
    b.at(0, 0)=1; b.at(1, 0)=2;
    a.solve(b).print();
}
```

Compile with `c++ -std=c++17 -Iinclude main.cpp -o app`, copy the headers, or use
CMake: `add_subdirectory(external/invariant)` and link `invariant::invariant`.
Installed consumers use `find_package(invariant 0.1 CONFIG REQUIRED)` with
`-DCMAKE_PREFIX_PATH="$HOME/.local"`.

Build options: `BUILD_TESTS`, `BUILD_EXAMPLES` (on for standalone builds),
`BUILD_BENCHMARKS`, `BUILD_PYTHON` (off). C++ use has no Python dependency.
Examples: `sample_usage`, `linear_fit`, `jacobi_interpolation`, `checked_solve`
under `build/examples` (or its `Release` subdirectory):

```sh
./build/examples/sample_usage
./build/examples/linear_fit
./build/examples/jacobi_interpolation
./build/examples/checked_solve
```

## Numerical contracts

- Solving and normalization require floating-point matrices. Indices and shapes
  are checked; empty matrices are supported except by solvers.
- `solve` uses scaled partial pivoting and accepts multiple RHS columns. A scaled
  pivot at most `epsilon(T)*rows` is rejected as numerically rank deficient.
- `least_squares` uses Householder QR, requires full column rank and rows ≥ columns,
  and supports multiple RHS columns. It does not compute a pseudoinverse.
- Jacobi stops at `||A*x-b||₂ <= tolerance*max(1, ||b||₂)`; iteration exhaustion
  throws. Default: 10,000 iterations and tolerance 1e-10 (choose a looser tolerance
  for float32). Ill-conditioned problems still require care with scaling/residuals.
- Invalid arguments/non-finite solver inputs throw `invalid_argument`; singular or
  rank-deficient systems throw `domain_error`; divergence throws `runtime_error`.
  Python maps these to `ValueError`, `ValueError`, and `RuntimeError` respectively.
  Bad indexing raises `out_of_range` / `IndexError`.
- Matrix powers require nonnegative exponents. Stochastic normalization validates
  nonnegative finite entries and positive column sums before modifying anything.
- Legacy C++ names `Add`, `Subtract`, and `LinearFit` remain. `LinearFit` performs
  polynomial interpolation, not least squares. Pointer APIs require valid storage
  of the declared size; integer arithmetic follows ordinary C++ overflow/division rules.

## Validation and release builds

Tests cover numerical/reference parity, invalid inputs, copy ownership, header
linking, examples, and a relocated CMake installation. CI runs C++ and Python
checks on Linux, macOS, and Windows, with a separate sanitizer job.

```sh
cmake -S . -B build/sanitized -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build/sanitized --parallel
ctest --test-dir build/sanitized --output-on-failure
python -m pip install build
python -m build  # source archive and platform-specific wheel in dist/
```

For C++ timing:

```sh
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
cmake --build build/release --config Release --parallel
./build/release/matrix_benchmark
```

The benchmark It uses seed 42, one warm-up, and seven repetitions, with
allocation included and correctness checks outside timing. Python benchmarks
include conversion copies. [Sample C++ measurements](docs/benchmark-macos-arm64.csv)
are machine-specific; neither benchmark establishes a universal speed advantage.

Neural-network examples live in [CPPNN](https://github.com/SimonVutov/CPPNN), including
a CIFAR-10 example using these bindings. Invariant implements numerical primitives.

MIT licensed; see [LICENSE](LICENSE). Inspired by ECE 204.
