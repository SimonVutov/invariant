# Copilot Instructions for AI Agents

## Project Overview

This repository is a header-only C++17 library focused on Matrix, Vector, and Polynomial operations, inspired by University of Waterloo ECE115 (Linear Algebra) and ECE204 (Numerical Methods) courses. Much of the code is adapted from course material by Douglas Wilhelm Harder.

## Key Architectural Concepts

- **Header-only library:** All core logic is in template headers under `include/invariant/`.
- **Focus:** Implements mathematical functions and algorithms relevant to linear algebra and numerical methods.
- **Goal:** Build a reusable C++ library, with potential future Python bindings via pybind11.

## Developer Workflows

- **Build:**
    - Uses CMake. From the project root:
        ```sh
        mkdir build && cd build
        cmake ..
        make
        ```
- **Testing:**
    - Run `./tests/test_matrix` from the build directory, or use `ctest`.
- **Debugging:**
    - Use standard C++ debugging tools (e.g., gdb, lldb).

## Project Conventions

- **C++17** is the minimum required standard.
- **Naming:** Functions and classes follow C++ idioms; mathematical concepts are named after their textbook or lecture equivalents.
- **Documentation:** See `readme.md` for project intent and background.
- **External dependencies:** None at present. Future plans may include pybind11 for Python integration.
- **Adding new functionality:** Add new methods to the appropriate header in `include/invariant/`. Follow the style of existing code (e.g., member methods on `Matrix<T>`).
- **Adding new examples:** Create a `.cpp` file in `examples/`, add it to `examples/CMakeLists.txt`, and document it in `readme.md`.

## Key Files

- `include/invariant/Matrix.hpp`: Matrix class with solvers (Gaussian elimination, Jacobi), operations, and utilities.
- `include/invariant/Polynomial.hpp`: Polynomial evaluation (Horner's method) and power functions.
- `include/invariant/invariant.hpp`: Convenience header that includes all modules.
- `examples/sample_usage.cpp`: Matrix operations demo.
- `examples/linear_fit.cpp`: Polynomial fitting demo.
- `examples/jacobi_interpolation.cpp`: Jacobi iterative solver and linear interpolation demo.
- `tests/test_matrix.cpp`: Unit tests for the Matrix class.
- `CMakeLists.txt`: Top-level build configuration.
- `readme.md`: Project background and usage guide.

---

For major changes, update this file to keep AI agents productive.
