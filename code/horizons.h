/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "platform.h"
#include "horizons_math.h"

typedef struct game_state
{
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 Initialized;
  
  s32 EntityCount;
  s32 StartChunkEntityIndex;
  
  platform_mesh Mesh;
  platform_sprite Sprite;
  platform_shader Shader;
  platform_shader FancyShader;
  
  b32 OnGround;
  f32 Speed;
  f32 FrictionCoefficient;
  f32 BounceDamping;
  
  vec2 CameraPos;
  f32 CameraWindowSize;
} game_state;

#endif //HORIZONS_H
