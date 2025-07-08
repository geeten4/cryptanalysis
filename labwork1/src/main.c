#include <assert.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


// S substitution
uint16_t S[] = {0x6, 0x4, 0xc, 0x5, 0x0, 0x7, 0x2, 0xe, 0x1, 0xf, 0x3, 0xd, 0x8, 0xa, 0x9, 0xb};
//                  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f
// S inverse
uint16_t invS[] = {0x4, 0x8, 0x6, 0xa, 0x1, 0x3, 0x0, 0x5, 0xc, 0xe, 0xd, 0xf, 0x2, 0xb, 0x7, 0x9};

// P permutation
uint16_t P[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
//                  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12,13,14, 15
// P inverse
uint16_t invP[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};


uint16_t *shortIntToFourBlocks(uint16_t x) {
    /*
        given a (16 bit) short int x, decompose it into four 4-bit blocks, each
        given in a separate short int, in a 4-element array
    */
   uint16_t *A = malloc(4 * sizeof(short int));
   for (int i = 0; i < 4; i++)
        A[i] = (x >> 4*(3 - i)) & 15;

   return A;
}

uint16_t shortIntSBox(uint16_t a) {
    /*
        given a = A[0] || A[1] || A[2] || A[3]
        returns u = S[A[0]] || S[A[1]] || S[A[2]] || S[A[3]]
    */
   
    // divide into blocks
   uint16_t *A = shortIntToFourBlocks(a);
    
   // combine back together
   return (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
}

uint16_t shortIntInvSBox(uint16_t u) {
    /*
        given u = U[0] || U[1] || U[2] || U[3]
        returns a = invS[U[0]] || invS[U[1]] || invS[U[2]] || invS[U[3]]
    */

    // divide into blocks
   uint16_t *U = shortIntToFourBlocks(u);
    
   // combine bUck together
   return (invS[U[0]] << 12) | (invS[U[1]] << 8) | (invS[U[2]] << 4) | invS[U[3]];
}

uint16_t *generateRoundKeys(int numberOfKeys) {
    /*
        returns ptr to array of numberOfKeys randomly generated uint16_ts
    */
    uint16_t *k = malloc(numberOfKeys * sizeof(short int));
    for (int i = 0; i < numberOfKeys; i++)
        k[i] = (short) rand();

    return k;
}

uint16_t bitPermute(uint16_t x) {
    /*
    * permutes the bits of x according to P
    */
    uint16_t result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << invP[i]) )
            result |= 1;
    }
    return result;
}

uint16_t inverseBitPermute(uint16_t x) {
    /*
    * permutes the bits of x according to P inverse
    */
    uint16_t result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << P[i]) )
            result |= 1;
    }
    return result;
}

uint16_t encryptCipherFour(uint16_t message, int roundCount, uint16_t *keys, bool lastRound) {
    /*
        given message as int, int roundCount as number of rounds to be iterated over
        and roundCount number of keys, returns encrypted message using CipherFour
    */

   uint16_t u = message, a, *A;

   // go through rounds
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
    if (!lastRound) {
        return u;
    }

    // last round
    a = u ^ keys[roundCount - 1];

    // divide into blocks
    A = shortIntToFourBlocks(a);

    // combine back together
    u = (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];

    // last xor
    return u ^ keys[roundCount];
}

uint16_t decryptCipherFour(int cypherText, int roundCount, uint16_t *keys) {
    uint16_t u = cypherText ^ keys[roundCount], *A = shortIntToFourBlocks(u);

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

void printBytes(uint16_t x) {
    for (int i = sizeof(short int) * 8 - 1; i >= 0 ; i--)
    {
        if ((x >> i) % 2 == 0)
            printf("%d", 0);
        else
            printf("%d", 1);

        if (i % 4 == 0) printf(" ");
    }
}

int arrayMaxValIndex(size_t *array, int length, int start_index) {
    int maxIndex = start_index;
    for (size_t i = start_index; i < length + start_index; i++)
    {
        if (array[i] > array[maxIndex])
            maxIndex = i;
    }

    return maxIndex;
}

typedef struct Structure
{
    // two messages s.t. after first round their difference is (0, 0, 2, 0)
    uint16_t first;
    uint16_t second;
} Structure;


Structure *findStructures(int s) {
    /*
        for each possible nibble of k_0 = 0, ..., 15 find s pairs (m_i, m_j) s.t.
        their difference after the first round of CipherFour is exactly (0, 0, 2, 0)

        returns array of 16 * s Structures i.e. the first s Structures correspond
        to s pairs (m_i, m_j) which have difference of (0, 0, 2, 0) after the first round
        using key k_0 with the third nibble 0000 etc.
    */
    Structure *structures = malloc(16 * s * sizeof(Structure));

    // m_i = (t_0, t_1, i, t_2)
    // randomly choose t_0, t_1, t_2 
    uint16_t t = (short) rand();
    uint16_t m = (((t >> 4) % 16) << 4) ^ t;

    // for k_0_nibble, i, j work with the lowest 4 bits
    size_t success_count = 0, try_count = 0;
    // uint16_t a_0, a_1, m_i, m_j, *pk_0_nibble;
    uint16_t m_i, m_j, *pk_0_nibble;

    for (uint16_t k_0_nibble = 0; k_0_nibble < 16; k_0_nibble++)
    {
        
        success_count = 0;
        try_count = 0;
        uint16_t i, j;
        while (success_count < s) {
            i = try_count / 16;
            j = try_count % 16;
            
            try_count++;
            if (try_count == 256) {
                printf("Could not find s pairs for (m_i, m_j)");
                break;
            }

            // skip i = j
            if (i == j) continue;
            
            m_i = m ^ ((i ^ k_0_nibble) << 4);
            m_j = m ^ ((j ^ k_0_nibble) << 4);
            pk_0_nibble = (uint16_t *) &k_0_nibble;
            encryptCipherFour(m_i, 2, pk_0_nibble, false);

            // now we need P(a_0) ^ P(a_1) = (0, 0, 2, 0) where P is the permutation
            // since it is linear and P(0000 0000 0010 0000) = 0000 0000 0010 0000
            // it is equivalent to a_0 ^ a_1 = (0, 0, 2, 0)
            if ((encryptCipherFour(m_i, 2, pk_0_nibble, false) ^ encryptCipherFour(m_j, 2, pk_0_nibble, false)) == 32) {
                structures[s * k_0_nibble + success_count].first = m_i;
                structures[s * k_0_nibble + success_count].second = m_j;
                success_count++;
            }
        }
    }

    return structures;
}

void firstExercise() {
    // Labwork 1.(i)
    printf("Labwork #1 (i): Implement encryption and decryption by CipherFour:\n");

    // number of rounds
    int r = 10;
    // message to encrypt
    uint16_t m = 1232;

    printf("message: %d\n", m);

    // generate round keys
    uint16_t *k = generateRoundKeys(r + 1);

    uint16_t cypherText = encryptCipherFour(m, r, k, true);
    printf("cypherText: %d\n", cypherText);
    
    printf("decrypted cypherText: %d\n", decryptCipherFour(cypherText, r, k));   
}

void secondExercise() {
    // Labwork 1. (ii)
    printf("Labwork #1 (ii): Implement differential attack without filtering:\n");

    // use the differential (0, 0, 2, 0) -> ? -> ? -> ? -> (0, 0, 2, 0)
    // (0, 0, 2, 0) = 0000 0000 0010 0000 = 32
    uint16_t offset = 32;
    
    // number of rounds
    int r = 5;
    
    // generate round keys
    uint16_t *k = generateRoundKeys(r + 1);

    for (int i = 0; i < r + 1; i++)
    {
        printf("key k_%d: ", i);
        printBytes(k[i]);
        printf("\n");
    }
    
    // due to the differential having 0 in postion 1, 2, and 4, we can really
    // only recover the 3rd 4bit block

    // initialize the counter for x
    // k_5 = (?, ?, x, ?)
    uint16_t *xCounter;
    xCounter = malloc(16 * sizeof(short int));
    for (int i = 0; i < 16; i++)
        xCounter[i] = 0;

    int successful = 0, numberOfMessages = 2 << 15;
    uint16_t offsetCounter, cypherText, cypherTextOffset, possible_k_r, y_0, y_1, a_0, a_1;
    for (int counter = 0; counter < numberOfMessages; counter++)
    {
        // offset message
        offsetCounter = counter ^ offset;
    
        cypherText = encryptCipherFour(counter, r, k, true);
        cypherTextOffset = encryptCipherFour(offsetCounter, r, k, true);

        for (int i = 0; i < 16; i++)
        {
            // try all possible values of x as third block of k_5 = (?, ?, x, ?)
            possible_k_r = i << 4;
            y_0 = cypherText ^ possible_k_r;
            y_1 = cypherTextOffset ^ possible_k_r;
            a_0 = shortIntInvSBox(y_0);
            a_1 = shortIntInvSBox(y_1);

            // a_0 + a_1 = u_0 + u_1
            // guessing the correct k_r (i.e. its third block x) will result
            // in a high probability of u_0 + u_1 = (0, 0, 2, 0)
            if ((a_0 ^ a_1) == 32) {
                xCounter[i]++;
            }

        }
    }

    for (int i = 0; i < 16; i++)
    {
        printf("possible third nibble of key k_5: ");
        printBytes(i << 4);
        printf(", probability %f\n", ((float) xCounter[i]) / numberOfMessages);
    }

    // Fig. 6.8 - test how many pairs obey the differential
    // encryptCipherFour needs to return without the last round 

    for (int counter = 0; counter < numberOfMessages; counter++)
    {
        // offset message
        offsetCounter = counter ^ offset;
    
        cypherText = encryptCipherFour(counter, r, k, true);
        cypherTextOffset = encryptCipherFour(offsetCounter, r, k, true);
        if ((cypherText ^ cypherTextOffset) == 32) {
            successful++;
        }
    }

    printf("%d number of messages out of %d obey the differential.\n", successful, numberOfMessages);
}

void thirdExercise() {
    // Labwork 1. (iii)
    printf("Labwork #1 (iii): Implement differential attack with filtering:\n");

    // use the differential (0, 0, 2, 0) -> ? -> ? -> ? -> (0, 0, 2, 0)
    // (0, 0, 2, 0) = 0000 0000 0010 0000 = 32
    uint16_t offset = 32;

    // number of rounds
    int r = 5;
    
    // generate round keys
    uint16_t *k = generateRoundKeys(r + 1);

    for (int i = 0; i < r + 1; i++)
    {
        printf("key k_%d: ", i);
        printBytes(k[i]);
        printf("\n");
    }
    
    // due to the differential having 0 in postion 1, 2, and 4, we can really
    // only recover the 3rd 4bit block

    // initialize the counter for x
    // k_5 = (?, ?, x, ?)
    uint16_t *xCounter;
    xCounter = malloc(16 * sizeof(short int));
    for (int i = 0; i < 16; i++)
    {
        xCounter[i] = 0;
    }
   

    int numberOfMessages = 2 << 15, filteredMessagesCount = 0;
    uint16_t offsetCounter, cypherText, cypherTextOffset, possible_k_r, y_0, y_1, a_0, a_1, cypherTextDifference;
    for (int counter = 0; counter < numberOfMessages; counter++)
    {
        // offset message
        offsetCounter = counter ^ offset;
    
        cypherText = encryptCipherFour(counter, r, k, true);
        cypherTextOffset = encryptCipherFour(offsetCounter, r, k, true);

        // apply filtering
        // in order for the differential to hold, we need
        // cypherText ^ cypherTextOffset = (0, 0, h, 0)
        // where h \in {1, 2, 9, a}
        cypherTextDifference = cypherText ^ cypherTextOffset;
        if (
            cypherTextDifference != 16 &&
            cypherTextDifference != 32 &&
            cypherTextDifference != 144 &&
            cypherTextDifference != 160
        ) continue;

        filteredMessagesCount++;

        for (int i = 0; i < 16; i++)
        {
            // try all possible values of x as third block of k_5 = (?, ?, x, ?)
            possible_k_r = i << 4;
            y_0 = cypherText ^ possible_k_r;
            y_1 = cypherTextOffset ^ possible_k_r;
            a_0 = shortIntInvSBox(y_0);
            a_1 = shortIntInvSBox(y_1);

            // a_0 + a_1 = u_0 + u_1
            // guessing the correct k_r (i.e. its third block x) will result
            // in a high probability of u_0 + u_1 = (0, 0, 2, 0)
            if ((a_0 ^ a_1) == 32) xCounter[i]++;
        }
    }

    printf("Filtered %d out of %d messages.\n", filteredMessagesCount, numberOfMessages);

    for (int i = 0; i < 16; i++)
    {
        printf("possible third nibble of key k_5: ");
        printBytes(i << 4);
        printf(", probability %f\n", ((float) xCounter[i]) / numberOfMessages);
    }
}

void fourthExercise(bool verbose) {
    // Labwork 1.(iv)
    printf("Labwork #1 (iv): Implement truncated differential attack\n");

    size_t s = 3;

    printf("Using block size s: %ld i.e. %ld of chosen plaintext\n", s, s * 16);

    // if k_5 = (a_0, a_1, a_2, a_3), at position i, count how many times a_i had the highest bias
    size_t k_5_nibbles_highest_bias[4] = {0};

    // count how many times third nibble of k_0 had the highest bias
    size_t k_0_nibble_highest_bias = 0;

    size_t experimentCount = 100;
    for (size_t experimentCounter = 0; experimentCounter < experimentCount; experimentCounter++)
    {

        Structure *structures = findStructures(s);

        uint16_t first, second, *pp;

        for (uint16_t p = 0; p < 16; p++)
        {
            for (int i = 0; i < s; i++)
            {
                first = structures[p * s + i].first;
                second = structures[p * s + i].second;
                pp = (uint16_t *) &p;
                // assert after first round we get a difference (0, 0, 2, 0)
                assert((encryptCipherFour(first, 2, pp, false) ^ encryptCipherFour(second, 2, pp, false)) == 32);
            }   
        }
        
        // number of rounds
        int r = 5;

        // generate round keys
        uint16_t *k = generateRoundKeys(r + 1);

        if (verbose) {
            for (int i = 0; i < r + 1; i++)
            {
                printf("key k_%d: ", i);
                printBytes(k[i]);
                printf("\n");
            }
        }
            
        uint16_t cypherText1, cypherText2;
            
        // counter for k_0 third nibble k_0_nibble
        size_t *k_0_nibble_counters = malloc(16 * sizeof(size_t));
        for (size_t i = 0; i < 16; i++)
            k_0_nibble_counters[i] = 0;
        
        // initialize counters for all nibbles of k_5
        // 4 * 16 = 64 counters, k_5 = (a_0, a_1, a_2, a_3)
        // then first 16 counters correspond to values a_0 = 0000, .... , 1111
        size_t *k_5_nibbles_counters = malloc(64 * sizeof(size_t));
        for (size_t i = 0; i < 64; i++)
            k_5_nibbles_counters[i] = 0;
        
        uint16_t a_1, a_2, shiftedCypherText1, shiftedCypherText2;

        // iterate over all k_0 nibble values
        for (size_t p = 0; p < 16; p++)
        {
            for (int i = 0; i < s; i++)
            {
                cypherText1 = encryptCipherFour(structures[s * p + i].first, r, k, true);
                cypherText2 = encryptCipherFour(structures[s * p + i].second, r, k, true);

                for (size_t k_5_nibbles_count = 0; k_5_nibbles_count < 4; k_5_nibbles_count++)
                {
                    for (uint16_t k_5_nibble_guess = 0; k_5_nibble_guess < 16; k_5_nibble_guess++)
                    {
                        // k_5_nibble_guess = guess nibble of k_5
                        // k_5_nibbles_count which nibble we are trying to guess
                        // if guessed correctly, then this characteristic holds
                        // invS[cypherText1 ^ k_5_nibble_guess] ^ invS[cypherText2 ^ k_5_nibble_guess] = * 0 * *
                        
                        // first shift the desired nibble to the end
                        shiftedCypherText1 = (cypherText1 >> (4 * (3 - k_5_nibbles_count))) % 16;
                        shiftedCypherText2 = (cypherText2 >> (4 * (3 - k_5_nibbles_count))) % 16;

                        a_1 = shortIntInvSBox(shiftedCypherText1 ^ k_5_nibble_guess);
                        a_2 = shortIntInvSBox(shiftedCypherText2 ^ k_5_nibble_guess);

                        if (((a_1 ^ a_2) & 4) == 0) {
                            k_5_nibbles_counters[16 * k_5_nibbles_count + k_5_nibble_guess]++;
                            k_0_nibble_counters[p]++;
                        }
                    }
                }
            }
        }

        if (verbose) {
            for (uint16_t i = 0; i < 64; i++)
            {
                printf("nibble a_%d guess ", i / 16);
                printBytes((i % 16) << 4 * (3 - (i / 16)));
                printf(" counter is %ld\n", k_5_nibbles_counters[i]);
                if((i+1) % 16 == 0) {
                    printf("\n");
                }
            }
        }

        // count k_0 nibble highest bias
        uint16_t nibble = (k[0] >> 4) % 16;
        if (k_0_nibble_counters[nibble] == k_0_nibble_counters[arrayMaxValIndex(k_0_nibble_counters, 16, 0)])
            k_0_nibble_highest_bias++;

        // count k_5 nibbles highest biases
        for (size_t i = 0; i < 4; i++)
        {
            nibble = (k[r] >> (12 - 4 * i)) % 16;
            if (k_5_nibbles_counters[nibble + i * 16] == k_5_nibbles_counters[arrayMaxValIndex(k_5_nibbles_counters, 16, i * 16)])
                k_5_nibbles_highest_bias[i]++;
        }

        if (verbose) {
            printf("\nk_0_nibble_counters:\n");
            for (uint16_t p = 0; p < 16; p++)
            {
                printf("k_0_nibble_counters guess :");
                printBytes(p << 4);
                printf(" counter is %ld\n", k_0_nibble_counters[p]);
            }
        }
    }

    printf("Third nibble of k_0 had the highest bias %ld out of a %ld experiments\n", k_0_nibble_highest_bias, experimentCount);

    printf("\nk_5_nibbles_counters: let k_5 = (a_0, a_1, a_2, a_3)\n");
    for (size_t i = 0; i < 4; i++)
        printf("Nibble a_%ld had the highest bias %ld out of a %ld experiments\n", i, k_5_nibbles_highest_bias[i], experimentCount);
}

int main()
{
    srand(time(NULL));   // Initialization, should only be called once.

    firstExercise();
    printf("\n");

    secondExercise();
    printf("\n");

    thirdExercise();
    printf("\n");

    fourthExercise(false);

    return 0;
}

