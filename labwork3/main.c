#include <assert.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#define FIELD_MODULUS 0x107b  // F_2^12 modulus x^12 + x^7 + x^6 + x^5 + x^3 + x + 1
#define FIELD_SIZE = 4096
typedef uint16_t gf2_12;  // Field element: 12 bits max

gf2_12 *permutation_F;

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

gf2_12 invPermutationPolynomial(gf2_12 x) {
    return gf_pow(x, 1 << 7);
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
        return addNodeRecursive(baseNode->right, newNode);
    else if ((baseNode->value < newNode->value) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->value > newNode->value) & (baseNode->left != 0))
        return addNodeRecursive(baseNode->left, newNode);
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

void printNodes(Node *node) {
    if(node->left != 0) {
        printNodes(node->left);
    }
    printf("value: %d, i: %ld, \n", node->value, node->i);
    if (node->right != 0) {
        printNodes(node->right);
    }
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

            if((f_i ^ f_j) != f_ipj) {
                return false;
            }
        }
    }
    return true;
}

gf2_12 encrypt(gf2_12 x, gf2_12 key) {
    // F(x + key) + key
    return gf_add(permutation_F[gf_add(x, key)], key);
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
        return addMessageRecursive(baseNode->right, newNode);
    else if ((baseNode->message < newNode->message) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->message > newNode->message) & (baseNode->left != 0))
        return addMessageRecursive(baseNode->left, newNode);
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
    if (baseNode->message == message)
        return baseNode;
    else if ((baseNode->message < message) && (baseNode->right == 0))
        return 0;
    else if ((baseNode->message < message) && (baseNode->right != 0))
        return findMessageCiphertextNode(baseNode->right, message);
    else if ((baseNode->message > message) && (baseNode->left == 0))
        return 0;
    else if ((baseNode->message > message) && (baseNode->left != 0))
        return findMessageCiphertextNode(baseNode->left, message);
    return 0;
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
    // printf("\ntestMessageCiphertextAgainstKeys\n");
    // printf("baseNode->message: %d\n", baseNode->message);
    // printf("baseNode->ciphertext: %d\n", baseNode->ciphertext);
    // printf("EDE_2(%d, %d, %d) == %d: %d\n", baseNode->message, key1, key2, baseNode->ciphertext, EDE_2(baseNode->message, key1, key2) == baseNode->ciphertext);
    if (EDE_2(baseNode->message, key1, key2) != baseNode->ciphertext)
        return 1;

    int left = 0, right = 0;
    if (baseNode->left != 0)
        left = testMessageCiphertextAgainstKeys(baseNode->left, key1, key2);
    
    if (baseNode->right != 0)
        right = testMessageCiphertextAgainstKeys(baseNode->right, key1, key2);
    
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
        return addBIndexRecursive(baseNode->right, newNode);
    else if ((baseNode->b <= newNode->b) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->b > newNode->b) & (baseNode->left != 0))
        return addBIndexRecursive(baseNode->left, newNode);
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
    // printf("\nfreeBIndex\n");
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

MessageCiphertextNode* randomMessageCiphertextTree(gf2_12 key1, gf2_12 key2, size_t t) {
    // generate 2^t message-ciphertext pairs, store it as a binary tree
    gf2_12 message = rand_gf2_12();
    MessageCiphertextNode *base_m_c = malloc(sizeof(MessageCiphertextNode));
    base_m_c->message = message;
    base_m_c->ciphertext = EDE_2(message, key1, key2);

    for (size_t i = 0; i < 1 << t; i++)
    {
        message = rand_gf2_12();
        addMessage(base_m_c, message, EDE_2(message, key1, key2));
    }

    return base_m_c;
}

void randomMessageCiphertextToFile(FILE *file, gf2_12 key1, gf2_12 key2, int t) {
    gf2_12 message;

    // write header line with keys
    fprintf(file, "key1: %d, key2: %d\n", key1, key2);

    // write messages to csv file
    for (size_t i = 0; i < 1 << t; i++)
    {
        message = rand_gf2_12();
        fprintf(file, "%d,%d\n", message, EDE_2(message, key1, key2));
    }

}

typedef struct MessageCipherTextBaseWithKeys {
    MessageCiphertextNode *baseNode;
    gf2_12 key1;
    gf2_12 key2;
} MessageCipherTextBaseWithKeys;

MessageCipherTextBaseWithKeys *readMessageCiphertextTreeFromFile(FILE *file) {
    // read from csv file
    char* line = NULL;
    size_t len = 0;
    
    // read the keys from the header line
    getline(&line, &len, file);
    int k1, k2;
    sscanf(line, "key1: %d, key2: %d", &k1, &k2);
    assert((k1 < (1 << 12)) && (k2 < (1 << 12)));
    gf2_12 key1 = (gf2_12)k1, key2 = (gf2_12)k2;

    // initial value for tree
    int m, c;
    getline(&line, &len, file);
    sscanf(line, "%d,%d", &m, &c);
    MessageCiphertextNode *base_m_c = malloc(sizeof(MessageCiphertextNode));
    base_m_c->message = (gf2_12)m;
    base_m_c->ciphertext = (gf2_12)c;

    // read message-ciphertext pairs
    while (getline(&line, &len, file) != -1) {
        sscanf(line, "%d,%d", &m, &c);
        // printf("message: %d, ciphertext: %d\n", m, c);
        addMessage(base_m_c, (gf2_12)m, (gf2_12)c);
    }

    MessageCipherTextBaseWithKeys *base_with_keys = malloc(sizeof(MessageCipherTextBaseWithKeys));
    base_with_keys->baseNode = base_m_c;
    base_with_keys->key1 = key1;
    base_with_keys->key2 = key2;
    return base_with_keys;
}

typedef struct KeyPair
{
    gf2_12 key1;
    gf2_12 key2;
    struct KeyPair *left;
    struct KeyPair *right;
} KeyPair;

int addKeyPairRecursive(KeyPair *kp_base, KeyPair *kp_new) {
    int32_t indexNew = (kp_new->key1 << 12) ^ kp_new->key2;
    int32_t indexBase = (kp_base->key1 << 12) ^ kp_base->key2;

    if ((indexNew < indexBase) && (kp_base->left != 0)){
        return addKeyPairRecursive(kp_base->left, kp_new);
        }
    else if ((indexNew < indexBase) && (kp_base->left == 0)){
        kp_base->left = kp_new;
        }
    else if ((indexNew > indexBase) && (kp_base->right != 0)){
        return addKeyPairRecursive(kp_base->right, kp_new);
        }
    else if ((indexNew > indexBase) && (kp_base->right == 0)){
        kp_base->right = kp_new;
        }
    else{
        return 1;}
    return 0;
}

int addKeyPair(KeyPair *kp_base, gf2_12 key1, gf2_12 key2) {
    KeyPair *kp = malloc(sizeof(KeyPair));
    kp->key1 = key1;
    kp->key2 = key2;
    kp->left = 0;
    kp->right = 0;
    return addKeyPairRecursive(kp_base, kp);
}

gf2_12* initPermumtation(FILE *file) {
    // read from csv file
    char* line = NULL;
    size_t len = 0;

    gf2_12* permutation = malloc(4096 * sizeof(gf2_12));

    int x;    
    // read message-ciphertext pairs
    while (getline(&line, &len, file) != -1) {
        getline(&line, &len, file);
        sscanf(line, "%d", &x);
        permutation[len] = x;
    }

    return permutation;
}

int main() {
    
    // initialize permutation F along with its inverse
    FILE *permutatino_file = fopen("labwork3/random_permutation.txt.csv", "r");
    permutation_F = initPermumtation(permutatino_file);


    srand(time(NULL));   // Initialization, should only be called once.

    // assert the permutation F is in fact a permutation
    // assert(isPermutation(permutation_F));  # TODO: rework
    
    // assert the permutation F^-1 is in fact a permutation
    assert(isPermutation(invPermutationPolynomial));

    // assert the permutation F is non-linear
    // assert(!(isLinear(permutation_F)));  # TODO: rework

    // return 0;

    // are inverses
    // TODO: reowrk
    // for (gf2_12 x = 0; x < 4096; x++)
    // {
    //     if (invPermutationPolynomial(permutation_F(x)) != x) {
    //         printf("invPermutationPolynomial(permutation_F(%d)) == %d != %d", x, invPermutationPolynomial(permutation_F(x)), x);
    //         return 1;
    //     }
    //     // assert(invPermutationPolynomial(permutation_F(x)) != x);
    // }

    // generate random keys and message-ciphertext pairs
    // gf2_12 key1 = rand_gf2_12(), key2 = rand_gf2_12();
    // MessageCiphertextNode *base_m_c = randomMessageCiphertextTree(key1, key2, 20);

    // read random keys and message-ciphertext pairs from a csv file
    FILE *file = fopen("message_ciphertext.csv", "r");
    MessageCipherTextBaseWithKeys *base_with_keys = readMessageCiphertextTreeFromFile(file);
    fclose(file);
    MessageCiphertextNode *base_m_c = base_with_keys->baseNode;
    gf2_12 key1 = base_with_keys->key1;
    gf2_12 key2 = base_with_keys->key2;

    // print keys
    printf("key1: %d, key2: %d\n", key1, key2);
    // print tree
    // printMessageCiphertextNode(base_m_c);
    // return 0;

    gf2_12 m_i, c_i, b_i;
    MessageCiphertextNode *msnode;
    BIndexNode base_b_i = {
        .b = 0,
        .index = -1,
        .left = 0,
        .right = 0
    };
    KeyPair kp_base = {0, 0, 0, 0};

    for (gf2_12 asd = 0; asd < (1 << 0); asd++)
    {
        // guess A = a, run through all values of k_1=i, compute m_i = decrypt(a, i),
        // if m_i is in base_m_c tree, then we find c_i, find b_i = decrypt(c_i, i)
        // and store the values (b_i, i) in the tree base_b_i

        // printf("a: %d\n", a);
        gf2_12 a = 94;

        // for (size_t i = 1; i < 4096; i++)
        for (size_t i = 3004; i < 3129; i++)
        {
            // k_1 = i is the guess for key1
            
            // find m_i in the tree base_m_c
            m_i = decrypt(a, (gf2_12)i);
            msnode = findMessageCiphertextNode(base_m_c, m_i);
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
        // return 0;

        // iterate over all values of k_2 to find b_j = decrypt(a, j)
        gf2_12 b_j;
        BIndexNode *binode;

        for (gf2_12 j = 0; j < 1 << 12; j++)
        {
            // k_2 = j is the guess for key2
            // if(j != 372)
            //     continue;
            b_j = decrypt(a, j);

            // TODO: returns exaclty one, but multiplicites are present???
            binode = findBIndexNode(&base_b_i, b_j);
            if (binode == 0)
                continue;
            
            printf("binode: %ld\n", binode->index);

            // we found a b_j s.t. b_j = decrypt(a, j)
            // test if EDE_2(m, i, j) = c holds for all 
            // message-cipthertext pairs in base_m_c
            gf2_12 index = (gf2_12) binode->index;
            if (testMessageCiphertextAgainstKeys(base_m_c, index, j) == 0) {
                if(kp_base.key1 == 0 && kp_base.key2 == 0 && kp_base.left == 0 && kp_base.right == 0) {
                    kp_base.key1 = index;
                    kp_base.key2 = j;
                    printf("possible key pair: key1: %d, key2: %d\n", index, j);
                } else {
                    if(addKeyPair(&kp_base, index, j) == 0) {
                        printf("possible key pair: key1: %d, key2: %d\n", index, j);
                    }
                }
            }
        }

        freeBIndex(&base_b_i);
        base_b_i.b = 0;
        base_b_i.index = -1;
        base_b_i.left = 0;
        base_b_i.right = 0;
    }
}

