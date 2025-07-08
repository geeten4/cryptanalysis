import random
import os
import ctypes

SHARED_LIBRARY_PATH = os.path.dirname(os.path.abspath(__file__)) + '/../../bin/labwork3/encryption.so'
DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS = os.path.dirname(os.path.abspath(__file__)) + '/../data/message_ciphertext.csv'

# Load the shared library
lib = ctypes.CDLL(SHARED_LIBRARY_PATH)


FIELD_SIZE = 4096

# generate keys
key1, key2 = random.sample(range(FIELD_SIZE), 2)

# generate 2^t message pairs
t = 10
message_count = 1 << t
messages = random.sample(range(FIELD_SIZE), message_count)

with open(DEST_FILE_MESSAGE_CIPHERTEXT_PAIRS, 'w') as fp:
    fp.write(f'key1: {key1}, key2: {key2}\n')

    for message in messages:
        fp.write(f'{message}, {lib.EDE_2(message, key1, key2)}\n')
