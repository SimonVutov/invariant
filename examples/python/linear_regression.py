"""Fit noisy observations with Invariant's C++ Householder QR solver."""
import numpy as np
import invariant

rng = np.random.default_rng(42)
x = np.linspace(-2, 2, 100)
y = 1.5 + 2.0*x + rng.normal(0, 0.1, x.size)
design = np.column_stack([np.ones_like(x), x])
coefficients = invariant.least_squares(design, y)
predictions = invariant.matmul(design, coefficients[:, None])[:, 0]
print(f"intercept={coefficients[0]:.4f}, slope={coefficients[1]:.4f}")
print(f"RMSE={np.sqrt(np.mean((predictions-y)**2)):.4f}")
np.testing.assert_allclose(coefficients, [1.5, 2.0], atol=0.05)
