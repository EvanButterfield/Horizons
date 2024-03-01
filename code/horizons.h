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

typedef struct game_entity
{
  game_sprites Sprite;
  vec2 Pos;
  f32 Angle;
} game_entity;

typedef struct game_state
{
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 Initialized;
  
  game_sprite Sprites[MAX_ENTITIES];
  game_entity Entities[MAX_ENTITIES];
  s32 EntityCount;
  s32 StartChunkEntityIndex;
} game_state;

#endif //HORIZONS_H
