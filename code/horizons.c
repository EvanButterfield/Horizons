#include "horizons.h"

#include "string.h"

global game_state *State;
global platform_api *Platform;

GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  State = (game_state *)Memory->PermanentStorage;
  Platform = &Memory->Platform;
  if(!State->Initialized)
  {
    InitializeArena(&State->PermArena, (u8 *)Memory->PermanentStorage + sizeof(game_state),
                    Memory->PermanentStorageSize - sizeof(game_state));
    InitializeArena(&State->TempArena, (u8 *)Memory->TempStorage, Memory->TempStorageSize);
    
    u32 Pixels[] = {0xffffffff};
    State->Sprite = Platform->CreateSprite(Pixels, 1, 1);
    State->Angle = 0;
    
    State->Initialized = true;
  }
  
  State->Angle += DeltaTime;
  mat4 Transform = Mat4CreateTransform((vec2){0, 0}, State->Angle, (vec2){100, 100}, WindowDimension);
  
  f32 Aspect = (f32)WindowDimension.Height / WindowDimension.Width;
  mat4 Orth = Mat4Orthographic(0, (f32)WindowDimension.Width,
                               (f32)WindowDimension.Height, 0,
                               0, 10);
  mat4 M = Mat4Mul(Transform, Orth);
  
  Platform->DrawSprite(State->Sprite, (f32 *)M.Elements);
  
  State->TempArena.Used = 0;
  return(false);
}