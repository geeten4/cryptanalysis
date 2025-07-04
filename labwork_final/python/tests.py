import random
import os
import ctypes

from sympy import randprime, is_primitive_root, primitive_root
import random

SHARED_LIBRARY_PATH = os.path.dirname(os.path.abspath(__file__)) + '/../../bin/labwork_final/dl.so'
lib = ctypes.CDLL(SHARED_LIBRARY_PATH)

# Generate a random prime in a range
# lower = 1000
# upper = 5000
# p = randprime(lower, upper)
# g = primitive_root(p)
beta = 10

p = 4987
g = 2

# g = 11
# p = 870871
# beta = 5872

dl = lib.solve_dl(g, beta, p, 50, True)
# dl = lib.solve_dl(11, 5872, 870871, 50, True)

print(f"prime number p={p}, generator g={g}, beta={beta}, dl: {dl}, {pow(g, dl, p)} == {beta}")




