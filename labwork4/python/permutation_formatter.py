import re
import os

"""
    from permutation_original.txt creates permutation.txt and permutation_inverse.txt,
    which are just arrays better readable for c
"""


PERMUTATION_ORIGINAL_FILE = os.path.dirname(os.path.abspath(__file__)) + '/../data/permutation_original.txt'
PERMUTATION_FILE = os.path.dirname(os.path.abspath(__file__)) + '/../data/permutation.txt'
PERMUTATION_INVERSE_FILE = os.path.dirname(os.path.abspath(__file__)) + '/../data/permutation_inverse.txt'

permutation_lines = [''] * 2048
permutation_inverse_lines = [''] * 2048

with open(PERMUTATION_ORIGINAL_FILE, 'r') as permutation_original, \
     open(PERMUTATION_FILE, 'w') as permutation_file, \
     open(PERMUTATION_INVERSE_FILE, 'w') as permutation_inverse_file:

    for line in permutation_original.readlines():
        search = re.search(r'<\[(.*?)\], \[(.*?)\]>', line)
        inp = int(''.join(search.group(1).split(',')).replace(' ', ''), 2)
        out = int(''.join(search.group(2).split(',')).replace(' ', ''), 2)
        permutation_lines[inp] = str(out)
        permutation_inverse_lines[out] = str(inp)

    for line in permutation_lines:
        permutation_file.write(f"{line}\n")

    for line in permutation_inverse_lines:
        permutation_inverse_file.write(f"{line}\n")
