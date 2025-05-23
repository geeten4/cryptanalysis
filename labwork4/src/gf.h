#ifndef GF_H
#define GF_H

#include <stdint.h> 

extern const int FIELD_SIZE;

typedef uint16_t gf2_11;  // field element: 11 bits max
typedef uint32_t double_gf2_11; // double field element, max 22 bits

gf2_11 rand_gf2_11();
double_gf2_11 rand_double_gf2_11();
gf2_11 double_right(double_gf2_11 x);
gf2_11 double_left(double_gf2_11 x);
double_gf2_11 create_double(gf2_11 left, gf2_11 right);

#endif // GF_H