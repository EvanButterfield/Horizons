/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "platform.h"
#include "horizons_math.h"

typedef struct game_state
{
  b32 Initialized;
  memory_arena PermArena;
  memory_arena TempArena;
  game_input LastInput;
  
  vec3 CameraPosition;
  vec3 CameraRotation;
  vec3 CameraFront;
  vec3 CameraUp;
  
  b32 ControllingCharacter;
  
  f32 CubeRot;
} game_state;

#endif //HORIZONS_H
