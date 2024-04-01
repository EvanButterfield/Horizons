/* date = February 4th 2024 3:57 pm */

#ifndef HORIZONS_H
#define HORIZONS_H

#include "horizons_platform.h"
#include "horizons_math.h"

typedef struct game_material
{
  vec4 Color;
  f32 Metallic;
  f32 Roughness;
} game_material;

typedef struct game_mesh
{
  platform_mesh Mesh;
  game_material *Material;
} game_mesh;

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
  
  game_mesh *CubeMeshes;
  game_mesh *ConeMeshes;
  f32 CubeRot;
} game_state;

#endif //HORIZONS_H
