# Copilot Instructions for AI Agents

## Project Overview

This repository is a C++ library focused on Matrix, Vector, and Polynomial operations, inspired by University of Waterloo ECE115 (Linear Algebra) and ECE204 (Numerical Methods) courses. Much of the code is adapted from course material by Douglas Wilhelm Harder.

## Key Architectural Concepts

- **Single-file structure:** All core logic currently resides in `main.cpp`.
- **Focus:** Implements mathematical functions and algorithms relevant to linear algebra and numerical methods.
- **Goal:** Build a static C++ library, with potential future Python bindings via pybind11.

## Developer Workflows

- **Build:**
    - No build scripts or configuration files are present. Compile manually, e.g.:
        ```sh
        g++ -std=c++17 -O2 -o main main.cpp
        ```
- **Testing:**
    - No formal test suite. Test by running the compiled binary and checking output.
- **Debugging:**
    - Use standard C++ debugging tools (e.g., gdb, lldb).

## Project Conventions

- **C++17** is the minimum required standard.
- **Naming:** Functions and classes follow C++ idioms; mathematical concepts are named after their textbook or lecture equivalents.
- **Documentation:** See `readme.md` for project intent and background.
- **External dependencies:** None at present. Future plans may include pybind11 for Python integration.

## Examples

- To compile and run:
    ```sh
    g++ -std=c++17 -O2 -o main main.cpp
    ./main
    ```
- To extend: Add new mathematical functions directly to `main.cpp` following the style of existing code.

## Key Files

- `main.cpp`: All source code and logic.
- `readme.md`: Project background and goals.

---

For major changes, update this file to keep AI agents productive.
