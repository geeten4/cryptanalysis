import random
import os
from gen_random_permutation import DEST_FILE_PERMUTATION, DEST_FILE_PERMUTATION_INVERSE
import ctypes

# Load the shared library
lib = ctypes.CDLL('./bin/encryption.so')

# with open(os.path.dirname(os.path.abspath(__file__)) + '/../data/random_permutation.txt', 'r'):
#     pass

# with open('./labwork3/data/random_permutation.txt', 'r'):
#     pass

# print(lib.permutation_F)
permutation_file = lib.fopen('./labwork3/data/random_permutation.txt', 'r')
lib.initPermutation(lib.permutation_F, permutation_file)
# inverse_permutation_file = lib.fopen('../data/random_inverse_permutation.txt', 'r')
# lib.initPermutation(lib.permutation_F_inverse, inverse_permutation_file)
# lib.EDE_2.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
# lib.EDE_2.restype = ctypes.c_int

print(lib.permutation_F())
# print(lib.encrypt(1, 2))

# print(lib.EDE_2(5, 10, 2))  # Output: 25


# DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS = os.path.dirname(os.path.abspath(__file__)) + '/../data/message_ciphertext.csv'
# FIELD_SIZE = 4096

# permutation = []
# with open(DEST_FILE_PERMUTATION, 'r') as fp:
#     for line in fp.readlines():
#         permutation.append(int(line))

# inverse_permutation = []
# with open(DEST_FILE_PERMUTATION_INVERSE, 'r') as fp:
#     for line in fp.readlines():
#         inverse_permutation.append(int(line))

# def encrypt(x: int, k: int) -> int:
#     return (permutation[(x + k) % FIELD_SIZE] + k) % FIELD_SIZE

# def decrypt(x: int, k: int) -> int:
#     return (inverse_permutation[(x + k) % FIELD_SIZE] + k) % FIELD_SIZE

# def EDE_2(message: int, k1: int, k2: int) -> int:
#     return encrypt(decrypt(encrypt(message, k1), k2), k1)

# for i in range(4096):
#     print(encrypt(decrypt(i, 390), 390))

# # generate 2^t message pairs
# t = 10
# messages = random.sample(range(FIELD_SIZE), 2 << t)

# # generate keys
# key1, key2 = random.sample(range(FIELD_SIZE), 2)

# with open(DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS, 'w') as fp:
#     fp.write(f'key1: {key1}, key2: {key2}\n')

