#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "gf.h"

gf2_12 encrypt(gf2_12 x, gf2_12 key);
gf2_12 decrypt(gf2_12 x, gf2_12 key);
gf2_12 EDE_2(gf2_12 m, gf2_12 k_1, gf2_12 k_2);


#endif // ENCRYPTION_H