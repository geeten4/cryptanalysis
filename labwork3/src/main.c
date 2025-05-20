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

#define LABOWRK3_RADOM_PERMUTATION_PATH "labwork3/data/random_permutation.txt"
#define LABOWRK3_RADOM_INVERSE_PERMUTATION_PATH "labwork3/data/random_inverse_permutation.txt"
#define MESSAGE_CIPHERTEXT_PATH "labwork3/data/message_ciphertext.csv"


int main() {
    
    // initialize permutation F along with its inverse
    FILE *permutation_file = fopen(LABOWRK3_RADOM_PERMUTATION_PATH, "r");
    initPermutation(permutation_F, permutation_file);
    fclose(permutation_file);

    FILE *permutation_inverse_file = fopen(LABOWRK3_RADOM_INVERSE_PERMUTATION_PATH, "r");
    initPermutation(permutation_F_inverse, permutation_inverse_file);
    fclose(permutation_inverse_file);

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
        if (permutation_F_inverse[permutation_F[x]] != x) {
            printf("permutation_F_inverse[permutation_F[%d]] == %d != %d", x, permutation_F_inverse[permutation_F[x]], x);
            return 1;
        }
        if (permutation_F[permutation_F_inverse[x]] != x) {
            printf("permutation_F[permutation_F_inverse[%d]] == %d != %d", x, permutation_F[permutation_F_inverse[x]], x);
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

    // print keys
    printf("key1: %d, key2: %d\n", key1, key2);
    // print tree
    printMessageCiphertextNode(base_m_c);
    return 0;

    printf("%d", EDE_2(21, 3128, 372));
    printf("%d", encrypt(decrypt(encrypt(21, 3128), 372), 3128));

    gf2_12 m_i, c_i, b_i;
    MessageCiphertextNode *msnode;
    BIndexNode base_b_i = {
        .b = 0,
        .index = -1,
        .left = 0,
        .right = 0
    };
    KeyPair kp_base = {0, 0, 0, 0};

    for (gf2_12 randomACount = 0; randomACount < (1 << 0); randomACount++)
    {
        // guess A = a, run through all values of k_1=i, compute m_i = decrypt(a, i),
        // if m_i is in base_m_c tree, then we find c_i, find b_i = decrypt(c_i, i)
        // and store the values (b_i, i) in the tree base_b_i

        // gf2_12 a = rand_gf2_12();
        gf2_12 a = 94;

        for (size_t i = 0; i < 4096; i++)
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
            
            // printf("binode: %ld\n", binode->index);

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

