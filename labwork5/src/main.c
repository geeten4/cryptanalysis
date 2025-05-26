#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "aes.h"


int main() {
    aes_state state = create_aes_state();
    for (size_t i = 0; i < 16; i++)
    {
        state[i] = i;
    }
    print_aes_state(state);
    ShiftRows(state);
    print_aes_state(state);
    
}
