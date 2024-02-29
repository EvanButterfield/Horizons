/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "platform.h"
#include "horizons_math.h"

typedef struct game_sprite
{
  vec2 Size;
  s32 Sprite;
} game_sprite;

typedef enum game_sprites
{
  SPRITES_SPRITE,
  SPRITES_GROUND,
  SPRITES_GROUND2,
  
  SPRITES_MAX
} game_sprites;

typedef struct game_state
{
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 Initialized;
  
  f32 Angle;
  game_sprite Sprites[NUM_ENTITIES];
} game_state;

#endif //HORIZONS_H
