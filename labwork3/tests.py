import ctypes
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
labwork = ctypes.CDLL(f'{current_dir}/labwork3.so')


# possible m_i: 3032, c_i: 3382
# possible b_i: 3116, i: 4081

# possible m_i: 880, c_i: 1457
# possible b_i: 3116, i: 4088

# message: 4094, ciphertext: 464, left: 94684327410960, right: 0
# possible key pair: key1: 1, key2: 2366

key1 = 1
key2 = 2366
m = 4094  # c = 1112
print(labwork.EDE_2(m, key1, key2))
# a = labwork.encrypt(m, key1)
# print(f"a: {a}")
# b = labwork.decrypt(a, key2)
# print(f"b: {b}")
# c = labwork.encrypt(b, key1)
# print(f"c: {c}")

# TODO:
# possible m_i: 21, c_i: 1112
# possible b_i: 1079, i: 3128
# possible m_i: 21, c_i: 1112
# possible b_i: 1079, i: 3129

# print(labwork.encrypt(1079, 0))