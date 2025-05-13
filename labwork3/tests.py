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

# 2033,962

# key1 = 3128
for key1 in range(1, 100):
    key2 = 372
    m = 2033  # c = 1112
    print(labwork.EDE_2(m, key1, key2))
    a = labwork.encrypt(m, key1)
    # print(f"a: {a}")
    b = labwork.decrypt(a, key2)
    # print(f"b: {b}")
    c = labwork.encrypt(b, key1)
    print(f"key1: {key1}, c: {c}")

print(labwork.encrypt(21, 1))
print(labwork.decrypt(2246, 1))
print(labwork.encrypt(1079, 1))

print(f"labwork.decrypt(a, key2)")

# TODO:
# possible m_i: 21, c_i: 1112
# possible b_i: 1079, i: 3128
# possible m_i: 21, c_i: 1112
# possible b_i: 1079, i: 3129

# print(labwork.encrypt(1079, 0))

# from numpy import fac
# from sympy import isprime, factor_list
# from sympy.abc import x

# print(factor_list(x**12+ 1))

# m = 2**12
# for i in range(m):
#     if isprime(i) and isprime(m-i):
#         print(i, m-i)