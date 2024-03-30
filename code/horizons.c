#include "horizons.h"
#include "string.h"

global game_state *State;
global platform_api *Platform;

internal void UpdatePhysics(s32 EntityIndex, f32 DeltaTime);

GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  State = (game_state *)Memory->PermanentStorage;
  Platform = &Memory->Platform;
  if(!State->Initialized)
  {
    InitializeArena(&State->PermArena, (u8 *)Memory->PermanentStorage + sizeof(game_state),
                    Memory->PermanentStorageSize - sizeof(game_state));
    InitializeArena(&State->TempArena, (u8 *)Memory->TempStorage, Memory->TempStorageSize);
    
    State->Mesh = Platform->GetDefaultMesh();
    State->Sprite = Platform->GetDefaultSprite();
    State->Shader = Platform->GetDefaultShader();
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  f32 Aspect = (f32)WindowDimension.Height / WindowDimension.Width;
  mat4 Orth = Mat4Orthographic(0, (f32)WindowDimension.Width,
                               (f32)WindowDimension.Height, 0,
                               0, 100);
  
  mat4 Perspective = Mat4Perspective(Aspect, 60, 0.01f, 100);
  // mat4 View = Mat4View((vec3){0, 0, 0}, (vec3){1, 1, 0}, (vec3){0, 1, 0});
  mat4 View = Mat4Identity();
  mat4 PrevM = Mat4Mul(View, Perspective);
  mat4 Transform3D = Mat4CreateTransform3D((vec3){0, -5, 5},
                                           (vec3){0, 45, 0},
                                           (vec3){1, 1, 1});
  mat4 M = Mat4Mul(Transform3D, PrevM);
  Platform->DrawMesh((f32 *)M.Elements);
  
  s32 DeltaTimeI = (s32)roundf(DeltaTime*1000);
  
  
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}