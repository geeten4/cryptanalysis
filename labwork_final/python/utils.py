from sympy import isprime, factorint, primitive_root

def find_prime_with_five_or_more_distinct_factors(start=20000000, window=20000000):
    for p in range(start - window, start + window):
        if isprime(p):
            factors = factorint(p - 1)
            if len(factors) >= 7:  # five or more distinct prime factors
                return p
    return None

p = find_prime_with_five_or_more_distinct_factors()

print(f"p = {p}, p - 1 = {p - 1}")
print(f"Factors of p - 1 = {' * '.join(str(q ** e) for q, e in factorint(p - 1).items())}:")

# Find a primitive root (generator)
print("Generator of GF(18061)^*:", primitive_root(p))
