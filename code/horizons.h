/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "platform.h"

typedef struct game_state
{
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 Initialized;
  
  s32 Sprite;
  f32 CosAngle;
  b32 CosUp;
  f32 SinAngle;
  b32 SinUp;
} game_state;

#endif //HORIZONS_H
