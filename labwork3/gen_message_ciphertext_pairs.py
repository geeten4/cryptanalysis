import random


DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS = 'labwork3/message_ciphertext.csv'
FIELD_SIZE = 4096

# read permutations
from gen_random_permutation import dest_file_permutation, dest_file_permutation_inverse

permutation = []
with open(dest_file_permutation, 'r') as fp:
    for line in fp.readlines():
        permutation.append(int(line))

inverse_permutation = []
with open(dest_file_permutation_inverse, 'r') as fp:
    for line in fp.readlines():
        inverse_permutation.append(int(line))

def encrypt(x: int, k: int) -> int:
    return (permutation[(x + k) % FIELD_SIZE] + k) % FIELD_SIZE

def decrypt(x: int, k: int) -> int:
    return (inverse_permutation[(x + k) % FIELD_SIZE] + k) % FIELD_SIZE

def EDE_2(message: int, k1: int, k2: int) -> int:
    return 

for i in range(4096):
    print(encrypt(decrypt(i, 390), 390))

# generate 2^t message pairs
t = 10
messages = random.sample(range(FIELD_SIZE), 2 << t)

# generate keys
key1, key2 = random.sample(range(FIELD_SIZE), 2)

with open(DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS, 'w') as fp:
    fp.write(f'key1: {key1}, key2: {key2}\n')

