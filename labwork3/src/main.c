#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "gf.h"
#include "mc.h"
#include "bindex.h"
#include "keypair.h"
#include "permutation.h"
#include "encryption.h"

#define MESSAGE_CIPHERTEXT_PATH "labwork3/data/message_ciphertext.csv"


int main() {
    printf("Labwork #3: Recover keys using a MITM attack\n");
    // prints out count of falsely suggested keys for each A=a
    bool verbose = false;

    srand(time(NULL));   // Initialization, should only be called once.

    // assert the permutation F is in fact a permutation
    assert(isPermutation(permutation_F));
    
    // assert the permutation F^-1 is in fact a permutation
    assert(isPermutation(permutation_F_inverse));

    // assert the permutation F is non-linear
    assert(!(isLinear(permutation_F)));

    // are inverses
    for (gf2_12 x = 0; x < 4096; x++)
    {
        if (permutation_F_inverse(permutation_F(x)) != x) {
            printf("permutation_F_inverse(permutation_F(%d)) == %d != %d", x, permutation_F_inverse(permutation_F(x)), x);
            return 1;
        }
        if (permutation_F(permutation_F_inverse(x)) != x) {
            printf("permutation_F(permutation_F_inverse(%d)) == %d != %d", x, permutation_F(permutation_F_inverse(x)), x);
            return 1;
        }
    }

    // read random keys and message-ciphertext pairs from a csv file
    FILE *file = fopen(MESSAGE_CIPHERTEXT_PATH, "r");
    MessageCipherTextBaseWithKeys *base_with_keys = readMessageCiphertextTreeFromFile(file);
    fclose(file);

    MessageCiphertextNode *base_m_c = base_with_keys->baseNode;
    gf2_12 key1 = base_with_keys->key1;
    gf2_12 key2 = base_with_keys->key2;

    printf("key1: %d, key2: %d\n", key1, key2);

    gf2_12 m_i, c_i, b_i;
    MessageCiphertextNode *msnode;
    BIndexNode base_b_i = {
        .b = 0,
        .index = -1,
        .left = 0,
        .right = 0
    };
    KeyPair kp_base = {0, 0, 0, 0};

    size_t randomACount = 1 << 10;
    for (gf2_12 randomACounter = 0; randomACounter < randomACount; randomACounter++)
    {
        // guess A = a, run through all values of k_1=i, compute m_i = decrypt(a, i),
        // if m_i is in base_m_c tree, then we find c_i, find b_i = decrypt(c_i, i)
        // and store the values (b_i, i) in the tree base_b_i
        gf2_12 a = rand_gf2_12();

        // count how many keys we are actually trying against all the message-ciphertext pairs
        size_t false_key_count = 0;

        for (size_t i = 0; i < 4096; i++)
        {
            // k_1 = i is the guess for key1
            
            // find m_i in the tree base_m_c
            m_i = decrypt(a, (gf2_12)i);
            msnode = findMessageCiphertextNode(base_m_c, m_i);
            if (msnode == 0)
                continue;

            // given m_i and its ciphertext, decrypt c_i to get b_i
            c_i = msnode->ciphertext;
            b_i = decrypt(c_i, (gf2_12) i);
            
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

            false_key_count++;

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

        if (verbose)
            printf("Number of false keys given by guess A=%d is %ld\n", a, false_key_count);

        freeBIndex(&base_b_i);
        base_b_i.b = 0;
        base_b_i.index = -1;
        base_b_i.left = 0;
        base_b_i.right = 0;
    }
}

