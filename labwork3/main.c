#include <assert.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#define FIELD_MODULUS 0x107b  // F_2^12 modulus x^12 + x^7 + x^6 + x^5 + x^3 + x + 1
typedef uint16_t gf2_12;  // Field element: 12 bits max


gf2_12 gf_add(gf2_12 a, gf2_12 b) {
    return a ^ b;
}

gf2_12 gf_mul(gf2_12 a, gf2_12 b) {
    uint32_t result = 0;

    for (int i = 0; i < 12; i++) {
        if ((b >> i) & 1)
            result ^= ((uint32_t)a << i);
    }

    // Modular reduction
    for (int i = 23; i >= 12; i--) {
        if (result & (1 << i)) {
            result ^= (FIELD_MODULUS << (i - 12));
        }
    }

    return (gf2_12)(result & 0xFFF);  // Mask to 12 bits
}

gf2_12 gf_pow(gf2_12 base, uint16_t exponent) {
    gf2_12 result = 1;
    while (exponent) {
        if (exponent & 1)
            result = gf_mul(result, base);
        base = gf_mul(base, base);
        exponent >>= 1;
    }
    return result;
}

gf2_12 permutation_F(gf2_12 x) {
    // represent F_2^12 as F_2[x] / f, where f = x^12 + x^7 + x^6 + x^5 + x^3 + x + 1
    // we can write these polynomials as integers < 2^12 modulo 0b0001000001111011 = 0x107b =  4219

    // use permutation polynomial g = x^3 + x + 1 as a non-linear permutation g: F_2^12 -> F_2^12
    // long int longerX = x % FIELD_MODULUS;
    // return (longerX * longerX * longerX + longerX + 1) % FIELD_MODULUS;

    // use permutation polynomial g = x^2^5 as a non-linear permutation g: F_2^12 -> F_2^12
    // we can then easily compute its inverse, since
    // (x^2^5)^2^7 = x^2^12 = x in F_2^12
    return gf_pow(x, 32);
}

gf2_12 invPermutationPolynomial(gf2_12 x) {
    return gf_pow(x, 128);
}

typedef struct Node
{
    size_t i;
    gf2_12 value;
    struct Node *left;
    struct Node *right;
} Node;

Node* addNode(Node *baseNode, size_t i, gf2_12 value) {
    Node *node = baseNode;
    Node *newNode = malloc(sizeof(Node));
    newNode->i = i;
    newNode->value = value;
    newNode->right = 0;
    newNode->left = 0;

    while (true) {
        if ((node->value < value) & (node->right == 0)) {
            node->right = newNode;
            break;
        }
        else if ((node->value < value) & (node->right != 0)) {
            node = node->right;
            break;
        }
        else if ((node->value > value) & (node->left == 0)) {
            node->left = newNode;
            break;
        }
        else if ((node->value > value) & (node->left != 0)) {
            node = node->left;
            break;
        }
        else {
            return node;
        }
    }

    return 0;
}

bool isPermutation(gf2_12 (*func)(gf2_12)) {
    Node baseNode = {0, 0, 0, 0};

    // check permutation_F is in fact a permutation
    for (size_t i = 1; i < 4096; i++)
    {
        gf2_12 f_i = func((gf2_12) i);
        Node *collisionNode = addNode(&baseNode, i, f_i);
        if(collisionNode != 0) {
            printf("colliding nodes: i: %ld, value: %d and i: %ld, value: %d", collisionNode->i, collisionNode->value, i, f_i);
            return false;
        }
    }
    return true;
}

void printBytes(gf2_12 x) {
    for (int i = sizeof(short int) * 8 - 1; i >= 0 ; i--)
    {
        if ((x >> i) % 2 == 0) {
            printf("%d", 0);
        } else {
            printf("%d", 1);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
    }
}

bool isLinear(gf2_12 (*func)(gf2_12)) {
    // check it is non-linear
    gf2_12 f_i, f_j, f_ipj;
    for (size_t i = 0; i < 4096; i++)
    {
        for (size_t j = 0; j < 4096; j++)
        {
            if (i == j)
                continue;

            f_i = func((gf2_12) i);
            f_j = func((gf2_12) j);
            f_ipj = func((gf2_12) i+j);

            if(f_i ^ f_j != f_ipj) {
                return false;
            }
        }
    }
    return true;
}

gf2_12 encrypt(gf2_12 x, gf2_12 key) {
    return gf_add(permutation_F(gf_add(x, key)), key);
}

gf2_12 decrypt(gf2_12 x, gf2_12 key) {
    return gf_add(invPermutationPolynomial(gf_add(x, key)), key);
}

gf2_12 EDE_2(gf2_12 m, gf2_12 k_1, gf2_12 k_2) {
    return encrypt(decrypt(encrypt(m, k_1), k_2), k_1);
}

typedef struct MessageCiphertextNode
{
    gf2_12 message;
    gf2_12 ciphertext;
    struct MessageCiphertextNode* left;
    struct MessageCiphertextNode* right;
} MessageCiphertextNode;

int addMessage(MessageCiphertextNode *baseNode, gf2_12 message, gf2_12 ciphertext) {
    MessageCiphertextNode *node = baseNode;
    MessageCiphertextNode *newNode = malloc(sizeof(MessageCiphertextNode));
    newNode->message = message;
    newNode->ciphertext = ciphertext;

    while (true) {
        if ((node->message < message) & (node->right == 0)) {
            node->right = newNode;
            break;
        }
        else if ((node->message < message) & (node->right != 0)) {
            node = node->right;
            break;
        }
        else if ((node->message > message) & (node->left == 0)) {
            node->left = newNode;
            break;
        }
        else if ((node->message > message) & (node->left != 0)) {
            node = node->left;
            break;
        }
        else {
            return 1;
        }
    }

    return 0;
}

MessageCiphertextNode* findNode(MessageCiphertextNode *baseNode, gf2_12 message) {
    MessageCiphertextNode *node = baseNode;

    while (true) {
        if (node->message == message) {
            return node;
        } else if ((node->message < message) & (node->right == 0)) {
            return 0;
        } else if ((node->message < message) & (node->right != 0)) {
            node = node->right;
            continue;
        } else if ((node->message > message) & (node->left == 0)) {
            return 0;
        } else if ((node->message > message) & (node->left != 0)) {
            node = node->left;
            continue;
        } else {
            return 0;
        }
    }
}

int main() {
    
    srand(time(NULL));   // Initialization, should only be called once.

    // assert the permutation F is in fact a permutation
    assert(isPermutation(permutation_F));

    // assert the permutation F is non-linear
    assert(!(isLinear(permutation_F)));

    // generate random keys
    gf2_12 key1 = (gf2_12) rand();
    gf2_12 key2 = (gf2_12) rand();

    // generate 2^t message-ciphertext pairs, store it as a binary tree
    size_t t = 20;
    gf2_12 message = (gf2_12) rand();
    MessageCiphertextNode base_m_c = {message, EDE_2(message, key1, key2)};

    for (size_t i = 0; i < 1 << t; i++)
    {
        message = (gf2_12) rand();
        addMessage(&base_m_c, message, EDE_2(message, key1, key2));
    }
    
    // guess A = a, run through all values of k_1=i, compute m_i = decrypt(a, i),
    // if m_i is in base_m_c tree, then we find c_i, find b_i = decrypt(c_i, i)
    // and store the values (b_i, i) in the tree base_b_i
    gf2_12 a = (gf2_12) rand(), m_i;
    MessageCiphertextNode *node;
    for (size_t i = 0; i < 1 << 12; i++)
    {
        m_i = decrypt(a, (gf2_12) i);
        node = findNode(&base_m_c, m_i);
        if (node == 0)
            continue;
        else {
            printf("%d, %d\n", m_i, node->ciphertext);
        }
    }
    
}

