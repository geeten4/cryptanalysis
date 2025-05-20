#ifndef MC_H
#define MC_H

#include <stddef.h>
#include <stdio.h>
#include "gf.h"

typedef struct MessageCiphertextNode
{
    gf2_12 message;
    gf2_12 ciphertext;
    struct MessageCiphertextNode* left;
    struct MessageCiphertextNode* right;
} MessageCiphertextNode;


typedef struct MessageCipherTextBaseWithKeys {
    MessageCiphertextNode *baseNode;
    gf2_12 key1;
    gf2_12 key2;
} MessageCipherTextBaseWithKeys;


int addMessageRecursive(MessageCiphertextNode *baseNode, MessageCiphertextNode *newNode);
int addMessage(MessageCiphertextNode *baseNode, gf2_12 message, gf2_12 ciphertext);
MessageCiphertextNode* findMessageCiphertextNode(MessageCiphertextNode *baseNode, gf2_12 message);
void printMessageCiphertextNode(MessageCiphertextNode *node);
int testMessageCiphertextAgainstKeys(MessageCiphertextNode *baseNode, gf2_12 key1, gf2_12 key2);
MessageCiphertextNode* randomMessageCiphertextTree(gf2_12 key1, gf2_12 key2, size_t t);
void randomMessageCiphertextToFile(FILE *file, gf2_12 key1, gf2_12 key2, int t);
MessageCipherTextBaseWithKeys *readMessageCiphertextTreeFromFile(FILE *file);


#endif  // MC_H