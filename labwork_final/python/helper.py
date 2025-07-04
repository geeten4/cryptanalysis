import random
import os
from sympy import randprime, is_primitive_root, primitive_root, mod_inverse
import random

q=4987
alpha=2
beta=10
factor_basis = [2, 3, 5, 7, 11, 13]
factors = [2, 3, 3, 277]

def discrete_log_brute_force(g, h, p):
    for x in range(1, p):
        if pow(g, x, p) == h:
            return x
    return None

# Example

for f in factor_basis:
    x = discrete_log_brute_force(alpha, f, q)
    print(f"{f}: {x} x % 2 = {x%2}, x % 3 = {x%3}, x % 277 = {x%277}, x % 4986 = {x % 4986} {pow(alpha, x, q)}")
