import random

def format_int_to_binary(m: int) -> str:
    line = bin(m).replace('0b', '')
    return '0' * (12 - len(line)) + line

dest_file_permutation = 'labwork3/random_permutation.txt'
dest_file_permutation_inverse = 'labwork3/random_inverse_permutation.txt'

# random permutation of GF(2^12)
# 0, ... , 4095
n = 2 ** 12
field = list(range(n))
random.shuffle(field)
with open(dest_file_permutation, 'w') as fp:
    for m in field:
        fp.write(f"{m}\n")
fp.close()

# calculate inverse
inverse = [-1 for _ in range(n)]
counter = 0
for m in field:
    inverse[m] = counter
    counter += 1

with open(dest_file_permutation_inverse, 'w') as fp:
    for m in inverse:
        fp.write(f"{m}\n")
fp.close()
