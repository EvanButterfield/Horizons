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

typedef struct aabb
{
  vec3 Max;
  vec3 Min;
} aabb;

typedef enum collider_type
{
  COLLIDER_NONE,
  COLLIDER_BOX
} collider_type;

typedef enum render_type
{
  RENDER_TYPE_NONE,
  RENDER_TYPE_SPRITE,
  RENDER_TYPE_MESH
} render_type;

typedef enum entity_flags
{
  ENTITY_FLAGS_NONE = 0,
  ENTITY_FLAGS_IS_CAMERA = 1,
  ENTITY_FLAGS_GENERATE_BASIC_COLLISIONS = 2,
  ENTITY_FLAGS_GENERATE_MESH_COLLISIONS = 4,
  ENTITY_FLAGS_MOVING = 8,
  ENTITY_FLAGS_EXCLUDE_FROM_COLLISION_DRAW = 16,
  ENTITY_FLAGS_ROTATE = 32
} entity_flags;

#define MAX_ENTITIES 32
typedef enum collision_direction
{
  COLLISION_DIRECTION_POS_X,
  COLLISION_DIRECTION_NEG_X,
  COLLISION_DIRECTION_POS_Y,
  COLLISION_DIRECTION_NEG_Y,
  COLLISION_DIRECTION_POS_Z,
  COLLISION_DIRECTION_NEG_Z
} collision_direction;

typedef struct entity
{
  entity_flags Flags;
  
  // NOTE: If ColliderType is set to COLLIDER_NONE, we assume no collision logic
  collider_type ColliderType;
  // TODO(evan): Have multiple AABBs for each mesh
  aabb AABB;
  // NOTE: For basic colliders that are things like spheres,
  //       just use the first value stored in BasicColliderSize
  vec3 BasicColliderSize;
  
  s32 OtherCollisionIndices[MAX_ENTITIES - 1];
  s32 NumCollisions;
  
  // NOTE: If RenderType is set to RENDER_TYPE_NONE, we assume no rendering logic
  render_type RenderType;
  // NOTE: If any of these are not set, we assume we are using the default one
  platform_sprite Sprite;
  game_mesh *Meshes;
  s32 MeshCount;
  platform_shader Shader;
  
  f32 Speed;
  
  vec3 Position;
  vec3 Rotation;
  vec3 Scale;
  
  f32 Sensitivity;
  f32 Near;
  f32 Far;
  f32 FOV;
  vec3 Front;
  vec3 Up;
  
  vec4 Color;
} entity;

typedef enum game_mode
{
  GAME_MODE_VIEW,
  GAME_MODE_FIRST_PERSON,
  GAME_MODE_DEV
} game_mode;

#define MAX_COLLIDERS 32
typedef struct game_state
{
  b32 Initialized;
  memory_arena PermArena;
  memory_arena TempArena;
  game_input LastInput;
  
  f32 Time;
  
  game_mode Mode;
  
  b32 IsColliding;
  
  f32 AmbientStrength;
  vec3 LightDirection;
  vec3 LightColor;
  
  platform_shader Shader2d;
  platform_shader OutlineShader;
  game_mesh OutlineMesh;
  
  b32 ControllingCharacter;
  
  game_mesh *CubeMeshes;
  s32 CubeMeshCount;
  game_mesh *ConeMeshes;
  s32 ConeMeshCount;
  
  entity Entities[MAX_ENTITIES];
  s32 EntityCount;
  
  s32 CurrentCameraIndex;
} game_state;

#endif //HORIZONS_H
