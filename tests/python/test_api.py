import gc
from concurrent.futures import ThreadPoolExecutor
import numpy as np
import pytest
import invariant as inv


@pytest.mark.parametrize("dtype,tol", [(np.float32, 2e-5), (np.float64, 1e-11)])
def test_kernels(dtype, tol):
    rng = np.random.default_rng(9)
    a = rng.normal(size=(9, 5)).astype(dtype)
    b = rng.normal(size=(5, 3)).astype(dtype)
    np.testing.assert_allclose(inv.matmul(a, b), a @ b, rtol=tol, atol=tol)
    rhs = rng.normal(size=(9, 2)).astype(dtype)
    result = inv.least_squares(a, rhs)
    assert result.dtype == dtype
    np.testing.assert_allclose(result, np.linalg.lstsq(a, rhs, rcond=None)[0], rtol=tol, atol=tol)
    square = a.T @ a + np.eye(5, dtype=dtype)
    np.testing.assert_allclose(inv.solve(square, b), np.linalg.solve(square, b), rtol=tol, atol=tol)
    np.testing.assert_allclose(inv.solve(square, b[:, 0]), np.linalg.solve(square, b[:, 0]), rtol=tol, atol=tol)
    assert inv.solve(square, b[:, 0]).shape == (5,)


@pytest.mark.parametrize("factory,dtype", [(inv.Matrix, np.float64), (inv.MatrixFloat, np.float32)])
def test_matrix(factory, dtype):
    source = np.arange(12, dtype=dtype).reshape(3, 4)[:, ::-1]
    a = factory(source)
    source[:] = -1
    assert a.shape == (3, 4)
    assert a[0, 0] == 3
    a[-1, -1] = 99
    copy = a.numpy()
    copy[:] = 0
    assert a[2, 3] == 99
    detached = a.numpy()
    del a
    gc.collect()
    assert detached[2, 3] == 99
    b = factory([[2., 0.], [0., 4.]])
    np.testing.assert_allclose((b @ b).numpy(), [[4, 0], [0, 16]])
    np.testing.assert_allclose((b + b - b).numpy(), b.numpy())
    np.testing.assert_allclose((2*b).numpy(), (b*2).numpy())
    np.testing.assert_allclose(b.T.numpy(), b.numpy().T)
    np.testing.assert_allclose(b.solve(factory([[4.], [8.]])).numpy(), [[2], [2]])
    np.testing.assert_allclose(b.power(0).numpy(), np.eye(2))
    with pytest.raises(IndexError):
        b[0, 2]
    with pytest.raises(IndexError):
        b[-3, 0] = 1
    with pytest.raises(ValueError):
        factory(-1, 2)
    with pytest.raises(ValueError):
        factory([1, 2])


def test_strides_and_shapes():
    a = np.arange(24.).reshape(4, 6)[::-1, ::2]
    b = np.asfortranarray(np.arange(6.).reshape(3, 2))
    np.testing.assert_allclose(inv.matmul(a, b), a @ b)
    assert inv.matmul(np.empty((3, 0)), np.empty((0, 2))).shape == (3, 2)
    assert inv.matmul(np.empty((0, 3)), np.ones((3, 2))).shape == (0, 2)
    assert inv.matmul([[1]], [[2]]).dtype == np.float64
    assert inv.matmul(np.ones((1, 1), np.float32), np.ones((1, 1), np.float64)).dtype == np.float64
    with pytest.raises(ValueError):
        inv.matmul([1, 2], [[1], [2]])
    with pytest.raises(ValueError):
        inv.matmul(np.ones((2, 3)), np.ones((2, 3)))
    with pytest.raises(TypeError):
        inv.matmul([[1j]], [[1]])


def test_failures():
    for solver in (inv.solve, inv.least_squares):
        with pytest.raises(ValueError):
            solver(np.ones((2, 2)), [1, 1])
        with pytest.raises(ValueError):
            solver([[np.nan]], [1])
        with pytest.raises(ValueError):
            solver(np.empty((0, 0)), [])
    with pytest.raises(ValueError):
        inv.least_squares(np.ones((2, 3)), [1, 1])
    with pytest.raises(RuntimeError):
        inv.solve_jacobi([[1, 2], [2, 1]], [1, 1], max_iterations=10)
    for value in (0, -1):
        with pytest.raises(ValueError):
            inv.solve_jacobi([[1]], [1], max_iterations=value)
    for value in (0, -1, np.inf, np.nan):
        with pytest.raises(ValueError):
            inv.solve_jacobi([[1]], [1], tolerance=value)
    np.testing.assert_allclose(inv.solve_jacobi([[4, 1], [1, 3]], [1, 2]), np.linalg.solve([[4, 1], [1, 3]], [1, 2]))
    with pytest.raises(ValueError):
        inv.Matrix([[1]]).power(-1)


def test_polynomial():
    assert inv.polyval([1, 2, 3], 2) == 17
    assert inv.polyval([7], 100) == 7
    with pytest.raises(ValueError):
        inv.polyval([], 2)
    with pytest.raises(ValueError):
        inv.polyval([[1, 2]], 2)


def test_concurrent_array_kernels():
    a = np.arange(64.).reshape(8, 8)
    with ThreadPoolExecutor(4) as pool:
        for value in pool.map(lambda _: inv.matmul(a, a), range(8)):
            np.testing.assert_allclose(value, a @ a)
