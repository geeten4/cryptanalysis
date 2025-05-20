import random
import os

DEST_FILE_PERMUTATION = os.path.dirname(os.path.abspath(__file__)) + '/../data/random_permutation.txt'
DEST_FILE_PERMUTATION_INVERSE = os.path.dirname(os.path.abspath(__file__)) + '/../data/random_inverse_permutation.txt'

def format_int_to_binary(m: int) -> str:
    line = bin(m).replace('0b', '')
    return '0' * (12 - len(line)) + line

# random permutation of GF(2^12)
# 0, ... , 4095
n = 2 ** 12
field = list(range(n))
random.shuffle(field)
with open(DEST_FILE_PERMUTATION, 'w') as fp:
    for m in field:
        fp.write(f"{m}\n")
fp.close()

# calculate inverse
inverse = [-1 for _ in range(n)]
counter = 0
for m in field:
    inverse[m] = counter
    counter += 1

with open(DEST_FILE_PERMUTATION_INVERSE, 'w') as fp:
    for m in inverse:
        fp.write(f"{m}\n")
fp.close()
