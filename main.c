#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// S substitution
unsigned int S[] = {0x6, 0x4, 0xc, 0x5, 0x0, 0x7, 0x2, 0xe, 0x1, 0xf, 0x3, 0xd, 0x8, 0xa, 0x9, 0xb};
//                  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f
// S inverse
unsigned int invS[] = {0x4, 0x8, 0x6, 0xa, 0x1, 0x3, 0x0, 0x5, 0xc, 0xe, 0xd, 0xf, 0x2, 0xb, 0x7, 0x9};

// P permutation
unsigned int P[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
//                  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12,13,14, 15
// P inverse
unsigned int invP[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};


unsigned short int *shortIntToFourBlocks(unsigned short int x) {
    /*
        given a (16 bit) short int x, decompose it into four 4-bit blocks, each
        given in a separate short int, in a 4-element array
    */
   unsigned short int *A = malloc(4 * sizeof(short int));
   for (int i = 0; i < 4; i++)
   {
        A[i] = (x >> 4*(3 - i)) & 15;
   }
   return A;
   
}

unsigned short int shortIntSBox(unsigned short int a) {
    /*
        given a = A[0] || A[1] || A[2] || A[3]
        returns u = S[A[0]] || S[A[1]] || S[A[2]] || S[A[3]]
    */
   
    // divide into blocks
   unsigned short int *A = shortIntToFourBlocks(a);
    
   // combine back together
   return (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
}

unsigned short int shortIntInvSBox(unsigned short int u) {
    /*
        given u = U[0] || U[1] || U[2] || U[3]
        returns a = invS[U[0]] || invS[U[1]] || invS[U[2]] || invS[U[3]]
    */

    // divide into blocks
   unsigned short int *U = shortIntToFourBlocks(u);
    
   // combine bUck together
   return (invS[U[0]] << 12) | (invS[U[1]] << 8) | (invS[U[2]] << 4) | invS[U[3]];
}

unsigned short int *generateRoundKeys(int numberOfKeys) {
    /*
        returns ptr to array of numberOfKeys randomly generated unsigned short ints
    */
    unsigned short int *k = malloc(numberOfKeys * sizeof(short int));
    for (int i = 0; i < numberOfKeys; i++)
    {
        k[i] = (short) rand();
    }
    return k;
}

void printArray(unsigned short int* array, int length) {
    for (int i = 0; i < length; i++)
    {
        printf("%d\n", array[i]);
    }
    
}

unsigned short int bitPermute(unsigned short int x) {
    /*
    * permutes the bits of x according to P
    */
    unsigned short int result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << invP[i]) )
            result |= 1;
    }
    return result;
}

unsigned short int inverseBitPermute(unsigned short int x) {
    /*
    * permutes the bits of x according to P inverse
    */
    unsigned short int result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << P[i]) )
            result |= 1;
    }
    return result;
}

unsigned short int encryptCipherFour(unsigned short int message, int roundCount, unsigned short int *keys) {
    /*
        given message as int, int roundCount as number of rounds to be iterated over
        and roundCount number of keys, returns encrypted message using CipherFour
    */

    // go through rounds
    unsigned short int u = message;
    unsigned short int a;
    unsigned short int *A;

    for (int i = 0; i < roundCount - 1; i++)
    {
        a = u ^ keys[i];
    
        // divide into blocks
        A = shortIntToFourBlocks(a);
    
        // combine back together
        u = (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
    
        // permute according to P
        u = bitPermute(u);
    }

    // return without last round
    // return u;

    // last round
    a = u ^ keys[roundCount - 1];

    // divide into blocks
    A = shortIntToFourBlocks(a);

    // combine back together
    u = (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];

    // last xor
    return u ^ keys[roundCount];
}

unsigned short int decryptCipherFour(int cypherText, int roundCount, unsigned short int *keys) {
    unsigned short int u = cypherText ^ keys[roundCount];
    unsigned short int *A = shortIntToFourBlocks(u);
    u = (invS[A[0]] << 12) | (invS[A[1]] << 8) | (invS[A[2]] << 4) | invS[A[3]];
    u ^= keys[roundCount - 1];

    for (int i = roundCount - 2; i >= 0 ; i--)
    {
        u = inverseBitPermute(u);
        A = shortIntToFourBlocks(u);
        u = (invS[A[0]] << 12) | (invS[A[1]] << 8) | (invS[A[2]] << 4) | invS[A[3]];
        u ^= keys[i];
    }
    
    return u;
}

void printBytes(unsigned short int x) {
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

unsigned short int *findStructures(int s) {
    /*
        for each possible nibble of k_0 = 0, ..., 15 find s pairs (m_i, m_j) s.t.
        their difference after the first round of CipherFour is exactly (0, 0, 2, 0)

        returns array of 2 * s * 16 short ints i.e. the first 2*s short ints correspond
        to s pairs (m_i, m_j) which have difference of (0, 0, 2, 0) after the first round
        using key k_0 with the third nibble 0 etc.
    */
    unsigned short int *structures = malloc(2 * s * 16 * sizeof(short int));

    // m_i = (t_0, t_1, i, t_2)
    // randomly choose t_0, t_1, t_2 
    unsigned short int t = (short) rand();
    unsigned short int m = (((t >> 4) % 16) << 4) ^ t;
    // return m;

    // for k_0_nibble, i, j work with the lowest 4 bits
    for (unsigned short int k_0_nibble = 0; k_0_nibble < 16; k_0_nibble++)
    {
            unsigned short int success_count = 0;
            unsigned short int i, j;
            int try_count = 0;
            while (success_count < s) {
                i = try_count / 16;
                j = try_count % 16;
                unsigned short int a_0 = i ^ k_0_nibble;
                unsigned short int a_1 = j ^ k_0_nibble;

                a_0 = shortIntSBox(a_0);
                a_1 = shortIntSBox(a_1);

                // now we need P(a_0) ^ P(a_1) = (0, 0, 2, 0) where P is the permutation
                // since it is linear and P(0000 0000 0010 0000) = 0000 0000 0010 0000
                // it is equivalent to a_0 ^ a_1 = (0, 0, 2, 0)

                if (((a_0 ^ a_1) % 16) == 2) {
                    // assign m_i and m_j
                    structures[s * k_0_nibble + 2 * success_count] = m ^ ((i % 16) << 4);
                    structures[s * k_0_nibble + 2 * success_count + 1] = m ^ ((j % 16) << 4);
                    success_count++;
                }

                try_count++;

                if (try_count == 256) {
                    printf("Could not find s pairs for (m_i, m_j)");
                    break;
                }
            }
    }

    return structures;
}

// Labwork 1.(iii)
int main()
{
    srand(time(NULL));   // Initialization, should only be called once.

    unsigned short int *structures = findStructures(1);
    for (int i = 0; i < 16; i++)
    {
        printBytes(structures[2 * i]);
        printf(", ");
        printBytes(structures[2 * i + 1]);
        printf("\n");
    }
    
    // printBytes(m);
}

// Labwork 1.(ii)
// int main() {

//     srand(time(NULL));   // Initialization, should only be called once.

//     // use the differential (0, 0, 2, 0) -> ? -> ? -> ? -> (0, 0, 2, 0)
//     // (0, 0, 2, 0) = 0000 0000 0010 0000 = 32
//     unsigned short int offset = 32;

    
//     // number of rounds
//     int r = 5;
    
//     // generate round keys
//     unsigned short int *k = generateRoundKeys(r + 1);

//     for (int i = 0; i < r + 1; i++)
//     {
//         printf("key k_%d: ", i);
//         printBytes(k[i]);
//         printf("\n");
//     }
    
//     // due to the differential having 0 in postion 1, 2, and 4, we can really
//     // only recover the 3rd 4bit block

//     // initialize the counter for x
//     // k_5 = (?, ?, x, ?)
//     unsigned short int *xCounter;
//     xCounter = malloc(16 * sizeof(short int));
//     for (int i = 0; i < 16; i++)
//     {
//         xCounter[i] = 0;
//     }
   

//     int successful = 0;
//     int numberOfMessages = 2 << 15;
//     unsigned short int offsetCounter, cypherText, cypherTextOffset, possible_k_r, y_0, y_1, a_0, a_1, cypherTextDifference;
//     for (int counter = 0; counter < numberOfMessages; counter++)
//     {
//         // offset message
//         offsetCounter = counter ^ offset;
    
//         cypherText = encryptCipherFour(counter, r, k);
//         cypherTextOffset = encryptCipherFour(offsetCounter, r, k);

//         // apply filtering
//         // in order for the differential to hold, we need
//         // cypherText ^ cypherTextOffset = (0, 0, h, 0)
//         // where h \in {1, 2, 9, a}
//         cypherTextDifference = cypherText ^ cypherTextOffset;
//         if (
//             cypherTextDifference != 16 &&
//             cypherTextDifference != 32 &&
//             cypherTextDifference != 144 &&
//             cypherTextDifference != 160
//         ) {
//             continue;
//         }

//         for (int i = 0; i < 16; i++)
//         {
//             // try all possible values of x as third block of k_5 = (?, ?, x, ?)
//             possible_k_r = i << 4;
//             y_0 = cypherText ^ possible_k_r;
//             y_1 = cypherTextOffset ^ possible_k_r;
//             a_0 = shortIntInvSBox(y_0);
//             a_1 = shortIntInvSBox(y_1);

//             // a_0 + a_1 = u_0 + u_1
//             // guessing the correct k_r (i.e. its third block x) will result
//             // in a high probability of u_0 + u_1 = (0, 0, 2, 0)
//             if ((a_0 ^ a_1) == 32) {
//                 xCounter[i]++;
//             }

//         }
//     }

//     for (int i = 0; i < 16; i++)
//     {
//         printf("possible key part: ");
//         printBytes(i << 4);
//         printf(", probability %f\n", ((float) xCounter[i]) / numberOfMessages);
//     }

//     return 0;
// }


// Labwork 1.(ii)
// int main() {

//     srand(time(NULL));   // Initialization, should only be called once.

//     // use the differential (0, 0, 2, 0) -> ? -> ? -> ? -> (0, 0, 2, 0)
//     // (0, 0, 2, 0) = 0000 0000 0010 0000 = 32
//     unsigned short int offset = 32;

    
//     // number of rounds
//     int r = 5;
    
//     // generate round keys
//     unsigned short int *k = generateRoundKeys(r + 1);

//     for (int i = 0; i < r + 1; i++)
//     {
//         printf("key k_%d: ", i);
//         printBytes(k[i]);
//         printf("\n");
//     }
    
//     // due to the differential having 0 in postion 1, 2, and 4, we can really
//     // only recover the 3rd 4bit block

//     // initialize the counter for x
//     // k_5 = (?, ?, x, ?)
//     unsigned short int *xCounter;
//     xCounter = malloc(16 * sizeof(short int));
//     for (int i = 0; i < 16; i++)
//     {
//         xCounter[i] = 0;
//     }
   

//     int successful = 0;
//     int numberOfMessages = 2 << 15;
//     unsigned short int offsetCounter;
//     unsigned short int cypherText;
//     unsigned short int cypherTextOffset;
//     unsigned short int possible_k_r;
//     unsigned short int y_0;
//     unsigned short int y_1;
//     unsigned short int a_0;
//     unsigned short int a_1;
//     for (int counter = 0; counter < numberOfMessages; counter++)
//     {
//         // offset message
//         offsetCounter = counter ^ offset;
    
//         cypherText = encryptCipherFour(counter, r, k);
//         cypherTextOffset = encryptCipherFour(offsetCounter, r, k);

//         for (int i = 0; i < 16; i++)
//         {
//             // try all possible values of x as third block of k_5 = (?, ?, x, ?)
//             possible_k_r = i << 4;
//             y_0 = cypherText ^ possible_k_r;
//             y_1 = cypherTextOffset ^ possible_k_r;
//             a_0 = shortIntInvSBox(y_0);
//             a_1 = shortIntInvSBox(y_1);

//             // a_0 + a_1 = u_0 + u_1
//             // guessing the correct k_r (i.e. its third block x) will result
//             // in a high probability of u_0 + u_1 = (0, 0, 2, 0)
//             if ((a_0 ^ a_1) == 32) {
//                 xCounter[i]++;
//             }

//         }
//     }

//     for (int i = 0; i < 16; i++)
//     {
//         printf("possible key part: ");
//         printBytes(i << 4);
//         printf(", probability %f\n", ((float) xCounter[i]) / numberOfMessages);
//     }

//     // printf("successful: %d, numberOfMessages: %d", successful, numberOfMessages);


//     // Fig. 6.8 - test how many pairs obey the differential
//     // encryptCipherFour needs to return without the last round 

//     // int successful = 0;
//     // int numberOfMessages = 2 << 15;
//     // for (int counter = 0; counter < numberOfMessages; counter++)
//     // {
//     //     // offset message
//     //     unsigned short int offset_counter = counter ^ offset;
    
//     //     unsigned short int cypherText = encryptCipherFour(counter, r, k);
//     //     unsigned short int cypherTextOffset = encryptCipherFour(offset_counter, r, k);
//     //     if ((cypherText ^ cypherTextOffset) == 32) {
//     //         successful++;
//     //     }
//     // }

//     // printf("successful: %d, numberOfMessages: %d", successful, numberOfMessages);
//     return 0;
// }


// Labwork 1.(i)
// int main() {

//     srand(time(NULL));   // Initialization, should only be called once.

//     // number of rounds
//     int r = 500;
//     // message to encrypt
//     unsigned short int m = 1232;

//     printf("message: %d\n", m);

//     // generate round keys
//     unsigned short int *k = generateRoundKeys(r + 1);

//     // print round keys
//     // printArray(k, r + 1);

//     unsigned short int cypherText = encryptCipherFour(m, r, k);
//     printf("cypherText: %d\n", cypherText);
    
//     printf("decrypted cypherText: %d\n", decryptCipherFour(cypherText, r, k));   

//     printf("\n");
//     return 0;

// }
