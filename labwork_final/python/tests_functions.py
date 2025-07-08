import datetime
import matplotlib.pyplot as plt
import numpy as np
import os
import ctypes
import time
import random
from sympy import randprime, primitive_root
import math

PLOTS_DIR =  os.path.dirname(os.path.abspath(__file__)) + '/../plots'

def print_usage():
    print("Usage: python script.py <arg1>")
    print("\n---------\n")
    print(f"<arg1> options:\n")
    
    # batch
    print(f"\tbatch <test_count> <lower> <fb_limit> <verbose>")
    print(f"\n\t\tperformes test_count number of tests")
    print(f"\t\twith random values of p \in [lower, 2*lower] range, with factor base upper bound")
    print(f"\t\tset to fb_limit. verbose set to either 'true' or 'false' enables more logging.")
    print(f"\n\t\texample usage:")
    print(f"\t\t\tbatch 20 100000000 theoretical true")
    print(f"\t\t\tbatch 20 100000000 50 true")
    print(f"\t\t\tbatch 2000 100000 20 false")
    print(f"\n")

    # fb_limit
    print(f"\tfb_limit <lower> <upper> <sample_count>")
    print(f"\n\t\tfind the optimal limit for factor basis by expection of average times")
    print(f"\t\tfor #sample_count number of random primes in range [lower, upper] performs")
    print(f"\t\tthe calculation for all factor base upper bounds up to a linear multiple")
    print(f"\t\tof sqrt(p). Plots the values against the theoretical limit.")
    print(f"\n\t\texample usage:")
    print(f"\t\t\tfb_limit 20 1000 5000")
    print(f"\t\t\tfb_limit 50000 100000 50")
    print(f"")

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


# optimal fb_limit should satisfy log(fb_limit) ~ sqrt(log(p) * log(log(p)))
def theoretical_fb_limit(p: int) -> int:
    return math.exp(math.sqrt(math.log(p) * math.log(math.log(p))))

def batch_test(test_count: int, lower: int, fb_limit: str, verbose: bool) -> None:
    """
        test #test_count values in the range [lower, 2 * lower]
    """

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

        if fb_limit == 'theoretical':
            tfb_limit = int(theoretical_fb_limit(p))
            if verbose:
                print(f"\tfb_limit={tfb_limit}")
            dl = lib.solve_dl(g, beta, p, tfb_limit, False)
        else:
            assert fb_limit.isnumeric()
            dl = lib.solve_dl(g, beta, p, int(fb_limit), False)


        elapsed = time.perf_counter() - start

        # Verify result
        check = pow(g, dl, p)
        if check != beta:
            print(f"[FAIL] p={p}, g={g}, beta={beta}, dl={dl}, g^dl % p = {check} != beta")
        elif verbose:
            print(f"[OK] p={p}, g={g}, beta={beta}, dl={dl} (time: {elapsed:.6f} sec)")

def fb_limit_test(lower: int, upper: int, sample_count: int):
    """
        Test optimal factor base limit given parameters
    """

    # number of random beta values per a prime p
    tests_per_p = 1

    # step size to increase the fb_limit
    fb_step = 1

    # linear multiple of log(p) up to which fb_limit is considered
    fb_fraction = 20

    optimal_data = []  # (q, optimal_fb_limit)

    print("\nComputing optimal fb_limit per p...")

    for _ in range(sample_count):
        p = randprime(lower, upper)
        g = primitive_root(p)
        fb_times = {}
        max_fb = int(math.log10(p) * fb_fraction)

        for _ in range(tests_per_p):
            beta = random.randint(1, p - 1)

            for fb in range(5, max_fb + 1, fb_step):
                start = time.perf_counter()
                dl = lib.solve_dl(g, beta, p, fb, False)
                elapsed = time.perf_counter() - start

                if pow(g, dl, p) == beta:
                    fb_times.setdefault(fb, []).append(elapsed)
                else:
                    print(f"[FAIL] p={p}, g={g}, beta={beta}, dl={dl}, g^dl % p = {pow(g, dl, p)} != beta")
                    break

        # Average time for each fb, only if it succeeded all trials
        avg_times = {fb: sum(times)/len(times) for fb, times in fb_times.items() if len(times) == tests_per_p}
        
        if avg_times:
            best_fb = min(avg_times, key=avg_times.get)
            optimal_data.append((p, best_fb))
            print(f"[OK] p={p}, optimal_fb_limit={best_fb}, max_fb: {max_fb}")

    # Plot results
    qs = [pt[0] for pt in optimal_data]
    optimal_fbs = [pt[1] for pt in optimal_data]

    # Theoretical curve: L_q(1/2) = exp(sqrt(log q * log log q))
    q_range = np.linspace(min(qs), max(qs), 300)
    L_q_half = np.exp(np.sqrt(np.log(q_range) * np.log(np.log(q_range))))

    plt.figure(figsize=(10, 6))
    plt.scatter(qs, optimal_fbs, label="Empirical Optimal fb_limit", color='blue', alpha=0.2)
    plt.plot(q_range, L_q_half, label=r'Theoretical $L_q(1/2)$', linestyle='--', color='green')

    plt.xlabel("Prime modulus q")
    plt.ylabel("Optimal fb_limit")
    plt.title("Empirical vs Theoretical Optimal fb_limit")
    plt.title("Empirical")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{PLOTS_DIR}/output_plot_{datetime.datetime.now()}.png", dpi=300)
