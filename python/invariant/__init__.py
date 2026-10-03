"""NumPy-friendly C++ numerics; array conversions always copy."""
import numpy as np
from . import _core
from ._core import Matrix, MatrixFloat, __version__

__all__ = ["Matrix", "MatrixFloat", "matmul", "solve", "least_squares", "solve_jacobi", "polyval", "__version__"]


def _compute(a, b, operation, max_iterations=10000, tolerance=1e-10):
    a, b = np.asarray(a), np.asarray(b)
    if a.dtype.kind not in "buif" or b.dtype.kind not in "buif":
        raise TypeError("Inputs must contain real numbers")
    vector = b.ndim == 1 and operation != "matmul"
    if vector:
        b = b[:, None]
    if a.ndim != 2 or b.ndim != 2:
        raise ValueError("Expected 2D matrices (solver RHS may be 1D)")
    dtype = np.float32 if a.dtype == b.dtype == np.float32 else np.float64
    kernel = _core.compute_f32 if dtype == np.float32 else _core.compute_f64
    result = kernel(np.asarray(a, dtype=dtype), np.asarray(b, dtype=dtype),
                    operation, max_iterations, tolerance)
    return result[:, 0] if vector else result


def matmul(a, b):
    """Multiply two 2D matrices in C++; float32 inputs preserve float32."""
    return _compute(a, b, "matmul")


def solve(a, b):
    """Solve square A X = B with scaled partial pivoting."""
    return _compute(a, b, "solve")


def least_squares(a, b):
    """Full-column-rank least squares via Householder QR; rows >= columns."""
    return _compute(a, b, "least_squares")


def solve_jacobi(a, b, *, max_iterations=10000, tolerance=1e-10):
    """Solve one RHS; raise RuntimeError if the residual fails to converge."""
    if not isinstance(max_iterations, int) or max_iterations <= 0:
        raise ValueError("max_iterations must be a positive integer")
    return _compute(a, b, "jacobi", max_iterations, tolerance)


def polyval(coefficients, x):
    """Evaluate ascending coefficients: [a, b, c] means a + b*x + c*x*x."""
    values = np.asarray(coefficients)
    if values.dtype.kind not in "buif":
        raise TypeError("Coefficients must be real numbers")
    return _core.polyval(values, float(x))
