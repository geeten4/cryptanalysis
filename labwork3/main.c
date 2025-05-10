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

Node *addNodeRecursive(Node *baseNode, Node *newNode) {

    if ((baseNode->value < newNode->value) & (baseNode->right != 0))
        addNodeRecursive(baseNode->right, newNode);
    else if ((baseNode->value < newNode->value) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->value > newNode->value) & (baseNode->left != 0))
        addNodeRecursive(baseNode->left, newNode);
    else if ((baseNode->value > newNode->value) & (baseNode->left == 0))
        baseNode->left = newNode;
    else
        return baseNode;
    return 0;
}

Node *addNode(Node *baseNode, size_t i, gf2_12 value) {
    Node *newNode = malloc(sizeof(Node));
    newNode->value = value;
    newNode->i = i;
    newNode->left = 0;
    newNode->right = 0;

    return addNodeRecursive(baseNode, newNode);
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
    // F(x + key) + key
    return gf_add(permutation_F(gf_add(x, key)), key);
}

gf2_12 decrypt(gf2_12 x, gf2_12 key) {
    // F^-1(x + key) + key
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

int addMessageRecursive(MessageCiphertextNode *baseNode, MessageCiphertextNode *newNode) {

    if ((baseNode->message < newNode->message) & (baseNode->right != 0))
        addMessageRecursive(baseNode->right, newNode);
    else if ((baseNode->message < newNode->message) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->message > newNode->message) & (baseNode->left != 0))
        addMessageRecursive(baseNode->left, newNode);
    else if ((baseNode->message > newNode->message) & (baseNode->left == 0))
        baseNode->left = newNode;
    else
        return 1;
    return 0;
}

int addMessage(MessageCiphertextNode *baseNode, gf2_12 message, gf2_12 ciphertext) {
    MessageCiphertextNode *newNode = malloc(sizeof(MessageCiphertextNode));
    newNode->message = message;
    newNode->ciphertext = ciphertext;
    newNode->left = 0;
    newNode->right = 0;

    return addMessageRecursive(baseNode, newNode);
}

MessageCiphertextNode* findMessageCiphertextNode(MessageCiphertextNode *baseNode, gf2_12 message) {
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

void printMessageCiphertextNode(MessageCiphertextNode *node) {
    if(node->left != 0) {
        printMessageCiphertextNode(node->left);
    }
    printf("message: %d, ciphertext: %d, left: %ld, right: %ld\n", node->message, node->ciphertext, (uintptr_t) node->left, (uintptr_t) node->right);
    if (node->right != 0) {
        printMessageCiphertextNode(node->right);
    }
}

int testMessageCiphertextAgainstKeys(MessageCiphertextNode *baseNode, gf2_12 key1, gf2_12 key2) {
    // returns 0 if key1 and key2 work for all message-ciphertext pairs in baseNode
    // returns 1 otherwise
    if (EDE_2(baseNode->message, key1, key2) != baseNode->ciphertext)
        return 1;
    
    int left = (baseNode->left == 0) ? 0 : testMessageCiphertextAgainstKeys(baseNode->left, key1, key2);
    int right = (baseNode->right == 0) ? 0 : testMessageCiphertextAgainstKeys(baseNode->right, key1, key2);
    return left || right;
}

typedef struct BIndexNode
{
    gf2_12 b;
    size_t index;
    struct BIndexNode* left;
    struct BIndexNode* right;
} BIndexNode;


int addBIndexRecursive(BIndexNode *baseNode, BIndexNode *newNode) {

    if ((baseNode->b <= newNode->b) & (baseNode->right != 0))
        addBIndexRecursive(baseNode->right, newNode);
    else if ((baseNode->b <= newNode->b) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->b > newNode->b) & (baseNode->left != 0))
        addBIndexRecursive(baseNode->left, newNode);
    else if ((baseNode->b > newNode->b) & (baseNode->left == 0))
        baseNode->left = newNode;
    else
        return 1;
    return 0;
}

int addBIndex(BIndexNode *baseNode, gf2_12 b, size_t index) {
    BIndexNode *newNode = malloc(sizeof(BIndexNode));
    newNode->b = b;
    newNode->index = index;
    newNode->left = 0;
    newNode->right = 0;

    return addBIndexRecursive(baseNode, newNode);
}

BIndexNode* findBIndexNode(BIndexNode *baseNode, gf2_12 b) {
    BIndexNode *node = baseNode;

    while (true) {
        if (node->b == b) {
            return node;
        } else if ((node->b < b) & (node->right == 0)) {
            return 0;
        } else if ((node->b < b) & (node->right != 0)) {
            node = node->right;
            continue;
        } else if ((node->b > b) & (node->left == 0)) {
            return 0;
        } else if ((node->b > b) & (node->left != 0)) {
            node = node->left;
            continue;
        } else {
            return 0;
        }
    }
}

void printBIndexNode(BIndexNode *node) {
    if(node->left != 0) {
        printBIndexNode(node->left);
    }
    printf("b: %d, index: %ld, left: %ld, right: %ld\n", node->b, node->index, (uintptr_t) node->left, (uintptr_t) node->right);
    if (node->right != 0) {
        printBIndexNode(node->right);
    }
}

void freeBIndex(BIndexNode *node) {
    if (node->left != 0) {
        freeBIndex(node->left);
        // printf("free node->left: %ld\n", (uintptr_t) node->left);
        free(node->left);
    }
    
    if (node->right != 0) {
        freeBIndex(node->right);
        // printf("free node->right: %ld\n", (uintptr_t) node->right);
        free(node->right);
    }
}

gf2_12 rand_gf2_12() {
    return (gf2_12) (rand() % (1 << 12));
}

int main() {
    
    srand(time(NULL));   // Initialization, should only be called once.

    // assert the permutation F is in fact a permutation
    assert(isPermutation(permutation_F));

    // assert the permutation F is non-linear
    assert(!(isLinear(permutation_F)));

    // are inverses
    for (gf2_12 x = 0; x < 4096; x++)
    {
        assert(invPermutationPolynomial(permutation_F(x)) == x);
    }

    // generate random keys
    gf2_12 key1 = rand_gf2_12(), key2 = rand_gf2_12();

    printf("key1: %d, key2: %d\n", key1, key2);

    // generate 2^t message-ciphertext pairs, store it as a binary tree
    size_t t = 12;
    gf2_12 message = rand_gf2_12();
    MessageCiphertextNode base_m_c = {message, EDE_2(message, key1, key2)};

    for (size_t i = 0; i < 1 << t; i++)
    {
        message = rand_gf2_12();
        addMessage(&base_m_c, message, EDE_2(message, key1, key2));
    }

    size_t randomACount = 1 << 1;

    gf2_12 a, m_i, c_i, b_i;
    MessageCiphertextNode *msnode;
    BIndexNode base_b_i = {
        .b = 0,
        .index = -1,
        .left = 0,
        .right = 0
    };
    for (size_t randomACounter = 0; randomACounter < randomACount; randomACounter++)
    {
        // guess A = a, run through all values of k_1=i, compute m_i = decrypt(a, i),
        // if m_i is in base_m_c tree, then we find c_i, find b_i = decrypt(c_i, i)
        // and store the values (b_i, i) in the tree base_b_i
        a = rand_gf2_12();
    
        for (size_t i = 0; i < 1 << 12; i++)
        {
            // k_1 = i is the guess for key1
            
            // find m_i in the tree base_m_c
            m_i = decrypt(a, (gf2_12) i);
            msnode = findMessageCiphertextNode(&base_m_c, m_i);
            if (msnode == 0)
                continue;

            // printf("possible m_i: %d, c_i: %d\n", msnode->message, msnode->ciphertext);
            // given m_i and its ciphertext, decrypt c_i to get b_i
            c_i = msnode->ciphertext;
            b_i = decrypt(c_i, (gf2_12) i);
            
            // printf("possible b_i: %d, i: %ld\n", b_i, i);
            // initialize base_b_i tree, use index=-1 to denote unitialized tree
            if (base_b_i.index == -1) {
                base_b_i.b = b_i;
                base_b_i.index = i;
                base_b_i.left = 0;
                base_b_i.right = 0;
                continue;
            }

            addBIndex(&base_b_i, b_i, i);
        }
        
        // printBIndexNode(&base_b_i);

        // iterate over all values of k_2 to find b_j = decrypt(a, j)
        gf2_12 b_j;
        BIndexNode *binode;

        for (gf2_12 j = 0; j < 1 << 12; j++)
        {
            // k_2 = j is the guess for key2
            b_j = decrypt(a, j);
            binode = findBIndexNode(&base_b_i, b_j);
            if (binode == 0)
                continue;
            
            // we found a b_j s.t. b_j = decrypt(a, j)
            // test if EDE_2(m, i, j) = c holds for all 
            // message-cipthertext pairs in base_m_c
            gf2_12 i = (gf2_12) binode->index;
            if (testMessageCiphertextAgainstKeys(&base_m_c, i, j) == 0) {
                printf("possible key pair: key1: %d, key2: %d\n", i, j);
            }
        }

        freeBIndex(&base_b_i);
        base_b_i.index = -1;
    }
}

