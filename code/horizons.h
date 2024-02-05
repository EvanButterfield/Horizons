/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "platform.h"

typedef struct game_state
{
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 Initialized;
} game_state;

#endif //HORIZONS_H
