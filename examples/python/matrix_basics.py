import numpy as np
from invariant import Matrix, solve, solve_jacobi, polyval

a = Matrix([[4., 1.], [1., 3.]])
b = np.array([1., 2.])
x = solve(a.numpy(), b)
print("solution:", x)
print("A squared:\n", (a @ a).numpy())
print("polynomial:", polyval([1, 2, 3], 2))
np.testing.assert_allclose(a.numpy() @ x, b)
np.testing.assert_allclose(solve_jacobi(a.numpy(), b), x)
