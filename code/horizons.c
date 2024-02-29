#include "horizons.h"
#include "string.h"
#include "hmp.h"
#include "world.h"

global game_state *State;
global platform_api *Platform;

global game_sprite Sprite;
global game_sprite Ground;
global game_sprite Ground2;

GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  State = (game_state *)Memory->PermanentStorage;
  Platform = &Memory->Platform;
  if(!State->Initialized)
  {
    InitializeArena(&State->PermArena, (u8 *)Memory->PermanentStorage + sizeof(game_state),
                    Memory->PermanentStorageSize - sizeof(game_state));
    InitializeArena(&State->TempArena, (u8 *)Memory->TempStorage, Memory->TempStorageSize);
    
    State->Sprites[SPRITES_SPRITE] =
      LoadHMP("corners.hmp", Platform, &State->PermArena);
    State->Sprites[SPRITES_GROUND] =
      LoadHMP("ground.hmp", Platform, &State->PermArena);
    State->Sprites[SPRITES_GROUND2] =
      LoadHMP("ground2.hmp", Platform, &State->PermArena);
    
    world_chunk *WorldChunks;
    s32 NumWorldChunks = LoadWorld("world", &WorldChunks, Platform, &State->TempArena, &State->PermArena);
    
    State->Angle = 0;
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  Sprite = State->Sprites[SPRITES_SPRITE];
  Ground = State->Sprites[SPRITES_GROUND];
  Ground2 = State->Sprites[SPRITES_GROUND2];
  
  f32 Aspect = (f32)WindowDimension.Height / WindowDimension.Width;
  mat4 Orth = Mat4Orthographic(0, (f32)WindowDimension.Width,
                               (f32)WindowDimension.Height, 0,
                               0, 10);
  mat4 Transform;
  mat4 M;
  
  State->Angle += DeltaTime;
  Transform = Mat4CreateTransform(false, (vec2){0, 0}, State->Angle,
                                  Sprite.Size, WindowDimension);
  M = Mat4Mul(Transform, Orth);
  Platform->DrawSprite(Sprite.Sprite, (f32 *)M.Elements);
  
  Transform = Mat4CreateTransform(false, (vec2){0, 300 + Ground.Size.y/2},
                                  0, Ground.Size, WindowDimension);
  M = Mat4Mul(Transform, Orth);
  Platform->DrawSprite(Ground.Sprite, (f32 *)M.Elements);
  
  Transform = Mat4CreateTransform(false, (vec2){Ground.Size.x, 300 + Ground2.Size.y/2},
                                  0, Ground2.Size, WindowDimension);
  M = Mat4Mul(Transform, Orth);
  Platform->DrawSprite(Ground2.Sprite, (f32 *)M.Elements);
  
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}