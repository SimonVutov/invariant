# Matrix benchmarks

Run the commands in the README to build `matrix_benchmark` in Release mode. The
CSV schema records operation, output rows, reduction dimension, output columns,
median milliseconds, and relative error when a reference is evaluated.

The program uses seed 42, one warm-up, and seven timed repetitions, reporting the
median. It includes result allocation and excludes input generation, reference
calculation, and the result checksum. Every timed result is consumed. Matrix
multiplication is checked against a separate scalar dot-product implementation;
solve is checked against a generated known solution. Error must be finite and
at most 1e-12. The dense-batch row is a timing-only workload (blank error field),
not a tested neural-network layer. Timings are not CI pass/fail thresholds.

Measured on 2026-09-17, Apple M5, macOS 26.6.2, AppleClang 21.0.0, CMake Release
(default `-O3 -DNDEBUG`), without BLAS, threading, or fast-math:

| Operation | Shape | Median milliseconds |
| --- | --- | ---: |
| Matrix multiplication | 32×32 × 32×32 | 0.009625 |
| Matrix multiplication | 128×128 × 128×128 | 0.617500 |
| Matrix multiplication | 256×256 × 256×256 | 2.643375 |
| Dense-batch matrix product | 128×784 × 784×32 | 0.661291 |
| Linear solve | 128×128, four RHS columns | 0.145958 |

[Raw CSV](benchmark-macos-arm64.csv) is included. Zero multiplication error on
this machine does not imply exact arithmetic: Apple ARM64 `long double` has the
same precision as `double`, and both implementations accumulate each dot product
in the same order. The loop order differs, so the reference still checks indexing.

These are one-machine measurements subject to scheduling, compiler, and clock
variation. They establish reproducible workloads; they do not establish a speedup
against the previous revision, NumPy, Eigen, BLAS, or CPPNN. Measure those baselines
on identical inputs and hardware before choosing a performance backend.
