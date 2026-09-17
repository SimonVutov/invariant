# Changelog

## 0.1.0 — release preparation (2026-09-17)

### Correctness

- Replace manual allocation with checked contiguous storage and exception-safe
  copy assignment; preserve valid moved-from objects.
- Check matrix indices, allocation dimensions, pointer inputs, and random ranges.
- Detect singular/numerically rank-deficient systems using scaled partial
  pivoting; reject non-finite solver input/output; support multiple RHS columns.
- Validate Jacobi controls, use a residual-based stopping criterion, and report
  nonconvergence instead of silently returning the last iterate.
- Reject unsupported negative matrix powers; allow powers on const matrices.
- Validate stochastic normalization before mutation, including nonnegative input.
- Fix polynomial forward declaration, header ODR violation, integer power template
  instantiation, negative integer powers, and null coefficient handling.
- Keep regression checks active with `NDEBUG`; exercise multi-file header linking.

### Packaging and examples

- Align source and CMake version at 0.1.0 (previous CMake declared 1.0.0).
- Export a relocatable CMake package, namespaced target, and C++17 requirement.
- Add installed-consumer testing, example tests, and a cross-platform CI workflow.
- Add an error-handling example, deterministic benchmarks, and revised usage docs.

### Compatibility notes

Previously undefined or silently incorrect operations now throw. Solvers and
normalization require floating-point types. Normalization rejects negative
entries. Jacobi callers must handle iteration exhaustion. Legacy names remain.
CMake 3.20+ is required for the documented test workflow; direct header consumers
still need only C++17.

### Verification

Locally tested using AppleClang 21 on Apple M5/macOS 26.6.2: Debug, Release, and
AddressSanitizer + UndefinedBehaviorSanitizer builds, including unit/regression
tests, four examples, and a relocated install consumed from a separate project.
Linux/Windows CI is configured but has not been run locally. This work has not
published a GitHub release or created a release tag.
