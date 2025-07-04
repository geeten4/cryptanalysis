import os
import ctypes
import time
import random
from sympy import randprime, primitive_root
import math

# Load shared library
SHARED_LIBRARY_PATH = os.path.dirname(os.path.abspath(__file__)) + '/../../bin/labwork_final/dl.so'
lib = ctypes.CDLL(SHARED_LIBRARY_PATH)

# Configure argument and return types for the C function
lib.solve_dl.argtypes = [ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int, ctypes.c_bool]
lib.solve_dl.restype = ctypes.c_ulonglong

# Test loop
test_count = 50
for i in range(test_count):
    # Random prime and generator
    lower = 100000
    p = randprime(lower, 2*lower)
    g = primitive_root(p)
    beta = random.randint(1, p - 1)

    # For fixed test:
    # p = 2503
    # g = 3
    # beta = 2342

    # print(f"testing p={p}, g={g}, beta={beta}")

    # Time the call
    start = time.perf_counter()
    dl = lib.solve_dl(g, beta, p, int(math.sqrt(lower)), False)
    elapsed = time.perf_counter() - start

    # Verify result
    check = pow(g, dl, p)
    if check != beta:
        print(f"[FAIL] p={p}, g={g}, beta={beta}, dl={dl}, g^dl % p = {check} != beta")
    # else:
    #     print(f"[OK] p={p}, g={g}, beta={beta}, dl={dl} (time: {elapsed:.6f} sec)")
