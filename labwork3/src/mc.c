#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "mc.h"
#include "encryption.h"


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
    if (EDE_2(baseNode->message, key1, key2) != baseNode->ciphertext)
        return 1;

    int left = 0, right = 0;
    if (baseNode->left != 0)
        left = testMessageCiphertextAgainstKeys(baseNode->left, key1, key2);
    
    if (baseNode->right != 0)
        right = testMessageCiphertextAgainstKeys(baseNode->right, key1, key2);
    
    return left || right;
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
        addMessage(base_m_c, (gf2_12)m, (gf2_12)c);
    }

    MessageCipherTextBaseWithKeys *base_with_keys = malloc(sizeof(MessageCipherTextBaseWithKeys));
    base_with_keys->baseNode = base_m_c;
    base_with_keys->key1 = key1;
    base_with_keys->key2 = key2;
    return base_with_keys;
}
