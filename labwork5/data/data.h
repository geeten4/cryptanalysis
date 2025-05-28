#ifndef S_BOXES_H
#define S_BOXES_H

#include <stdint.h>
#include "../src/gf.h"

extern gf2_8 S_box[16][16];
extern gf2_8 S_box_inv[16][16];

#endif  // S_BOXES_H