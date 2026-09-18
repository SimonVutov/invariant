# Changelog

## 0.1.0

- Checked matrix ownership, indexing, dimensions, and numerical error handling.
- Scaled-pivot linear solves, multiple RHS columns, and Householder QR least squares.
- Jacobi residual checks and explicit nonconvergence errors.
- Fixed polynomial header linking and power edge cases.
- Python float32/float64 bindings, NumPy array functions, type hints, and examples.
- Relocatable CMake installation, Python wheel/source builds, CI, and regression tests.
- Reproducible C++ and Python benchmarks.

Compatibility: invalid operations that previously returned incorrect values now
throw. Solvers require floating-point types. Negative matrix powers are rejected.
Legacy C++ API names are retained. Python requires 3.10+; CMake requires 3.20+.
