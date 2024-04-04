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
  vec3 *CollisionPositions;
  s32 CollisionPositionCount;
  game_material *Material;
} game_mesh;

typedef struct game_aabb
{
  vec3 Max;
  vec3 Min;
  vec3 MidPoint;
} game_aabb;

typedef struct game_state
{
  b32 Initialized;
  memory_arena PermArena;
  memory_arena TempArena;
  game_input LastInput;
  
  f32 Time;
  
  vec3 CameraPosition;
  vec3 CameraRotation;
  vec3 CameraFront;
  vec3 CameraUp;
  
  f32 AmbientStrength;
  vec3 LightDirection;
  vec3 LightColor;
  
  b32 ControllingCharacter;
  
  b32 Rotating;
  
  game_mesh *CubeMeshes;
  game_mesh *ConeMeshes;
  f32 CubeRot;
} game_state;

#endif //HORIZONS_H
