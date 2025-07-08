import matplotlib.pyplot as plt
import numpy as np
import os
import ctypes
import time
import random
from sympy import randprime, primitive_root
import math

# Load shared library
SHARED_LIBRARY_PATH = os.path.dirname(os.path.abspath(__file__)) + '/../../bin/labwork_final/dl.so'
lib = ctypes.CDLL(SHARED_LIBRARY_PATH)

# Import the C function to solve the DL
# gf_t solve_dl(gf_t alpha, gf_t beta, gf_t q, int fb_limit, bool verbose);

# returns d s.t. alpha ^ d == beta (mod q). 
# fb_limit: factor base limit. Upper bound on the highers number in factor basis
# verbose: enables debug logs

# Configure argument and return types for the C function
lib.solve_dl.argtypes = [ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int, ctypes.c_bool]
lib.solve_dl.restype = ctypes.c_ulonglong

# Simple test loop, tests random #test_count of prime numbers q in range [lower, 2 * lower]
# as base, random alpha (assume it is primitive), random beta

# set to current limit before integers overflowing
test_count = 20
lower = 1000000000

# optimal fb_limit should satisfy log(fb_limit) ~ sqrt(log(p) * log(log(p)))
def theoretical_fb_limit(p: int) -> int:
    return math.exp(math.sqrt(math.log(p) * math.log(math.log(p))))

print(f"Testing random values for {test_count} tests within range [{lower}, {2 * lower}]")
for i in range(test_count):
    # Random prime and generator
    p = randprime(lower, 2*lower)
    g = primitive_root(p)
    beta = random.randint(1, p - 1)

    # Time the call
    start = time.perf_counter()

    # theoretical limit not so good?
    # TODO: compare constant 50 with theoretical_fb_limit(p)
    
    # Constant 50
    # dl = lib.solve_dl(g, beta, p, 50, False)
    
    # Theoretical
    print(f"theoretical_fb_limit(p): {theoretical_fb_limit(p)}")
    dl = lib.solve_dl(g, beta, p, int(theoretical_fb_limit(p)), False)
    
    elapsed = time.perf_counter() - start

    # Verify result
    check = pow(g, dl, p)
    if check != beta:
        print(f"[FAIL] p={p}, g={g}, beta={beta}, dl={dl}, g^dl % p = {check} != beta")
    else:
        print(f"[OK] p={p}, g={g}, beta={beta}, dl={dl} (time: {elapsed:.6f} sec)")


# Test optimal factor base limit given parameters
sample_count = 50
lower = 50000
upper = 100000
tests_per_p = 1
fb_step = 1
fb_fraction = 0.6

optimal_data = []  # (q, optimal_fb_limit)

print("\nComputing optimal fb_limit per q...")

for _ in range(sample_count):
    p = randprime(lower, upper)
    g = primitive_root(p)
    max_fb = int(math.sqrt(p) * fb_fraction)

    fb_times = {}  # fb_limit → list of runtimes

    for _ in range(tests_per_p):
        beta = random.randint(1, p - 1)

        for fb in range(5, max_fb + 1, fb_step):
            start = time.perf_counter()
            dl = lib.solve_dl(g, beta, p, fb, False)
            elapsed = time.perf_counter() - start

            if pow(g, dl, p) == beta:
                fb_times.setdefault(fb, []).append(elapsed)

    # Average time for each fb, only if it succeeded all trials
    avg_times = {fb: sum(times)/len(times) for fb, times in fb_times.items() if len(times) == tests_per_p}
    
    if avg_times:
        best_fb = min(avg_times, key=avg_times.get)
        optimal_data.append((p, best_fb))
        print(f"[OK] p={p}, optimal_fb_limit={best_fb}")

# Plot results
qs = [pt[0] for pt in optimal_data]
optimal_fbs = [pt[1] for pt in optimal_data]

# Theoretical curve: L_q(1/2) = exp(sqrt(log q * log log q))
q_range = np.linspace(min(qs), max(qs), 300)
L_q_half = np.exp(np.sqrt(np.log(q_range) * np.log(np.log(q_range))))

plt.figure(figsize=(10, 6))
plt.scatter(qs, optimal_fbs, label="Empirical Optimal fb_limit", color='blue', alpha=0.7)
plt.plot(q_range, L_q_half, label=r'Theoretical $L_q(1/2)$', linestyle='--', color='green')

plt.xlabel("Prime modulus q")
plt.ylabel("Optimal fb_limit")
plt.title("Empirical vs Theoretical Optimal fb_limit")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()