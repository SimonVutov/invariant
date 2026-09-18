"""End-to-end NumPy-array API timing, including conversion copies."""
import argparse
import platform
import time
import numpy as np
import invariant

parser = argparse.ArgumentParser()
parser.add_argument("--size", type=int, default=128)
parser.add_argument("--repeats", type=int, default=7)
args = parser.parse_args()
if args.size <= 0 or args.repeats <= 0:
    parser.error("size and repeats must be positive")
rng = np.random.default_rng(42)
a, b = [rng.normal(size=(args.size, args.size)) for _ in range(2)]
reference = a @ b
print(f"# {platform.platform()}, numpy={np.__version__}, invariant={invariant.__version__}")
print("backend,size,median_ms,max_error")
for name, multiply in [("invariant", invariant.matmul), ("numpy", np.matmul)]:
    result = multiply(a, b)
    np.testing.assert_allclose(result, reference, rtol=1e-10, atol=1e-10)
    durations = []
    for _ in range(args.repeats):
        start = time.perf_counter()
        result = multiply(a, b)
        durations.append(time.perf_counter()-start)
    print(f"{name},{args.size},{1000*np.median(durations):.6f},{np.max(np.abs(result-reference)):.3g}")
