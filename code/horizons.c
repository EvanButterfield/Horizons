#include "horizons.h"
#include "string.h"
#include "hmp.h"
#include "world.h"

global game_state *State;
global platform_api *Platform;

global s32 SpriteEntity;

internal s32
CreateEntity(game_entity Entity)
{
  s32 Index = State->EntityCount;
  State->Entities[State->EntityCount++] = Entity;
  return(Index);
}

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
    if(NumWorldChunks == 0)
    {
      Platform->LogMessagePlain("No world chunks loaded. Exiting...\n", true, MESSAGE_SEVERITY_ERROR);
      return(true);
    }
    
    SpriteEntity = CreateEntity((game_entity){SPRITES_SPRITE});
    
    {
      State->StartChunkEntityIndex = State->EntityCount;
      f32 ChunkX = 0;
      for(s32 ChunkIndex = 0; ChunkIndex < NumWorldChunks; ++ChunkIndex)
      {
        world_chunk Chunk = WorldChunks[ChunkIndex];
        game_entity Entity;
        Entity.Sprite = Chunk.SpriteIndex;
        Entity.Pos.x = ChunkX;
        Entity.Pos.y = Chunk.Y;
        
        ChunkX += State->Sprites[SPRITES_GROUND].Size.x;
        
        CreateEntity(Entity);
      }
    }
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  State->Entities[SpriteEntity].Angle += DeltaTime;
  
  f32 Aspect = (f32)WindowDimension.Height / WindowDimension.Width;
  mat4 Orth = Mat4Orthographic(0, (f32)WindowDimension.Width,
                               (f32)WindowDimension.Height, 0,
                               0, 10);
  /*mat4 Transform;
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
  Platform->DrawSprite(Ground2.Sprite, (f32 *)M.Elements);*/
  
  for(s32 EntityIndex = 0; EntityIndex < State->EntityCount; ++EntityIndex)
  {
    game_entity Entity = State->Entities[EntityIndex];
    game_sprite Sprite = State->Sprites[Entity.Sprite];
    Entity.Pos.y += Sprite.Size.y/2;
    
    mat4 Transform = Mat4CreateTransform(false, Entity.Pos, Entity.Angle, Sprite.Size, WindowDimension);
    mat4 M = Mat4Mul(Transform, Orth);
    Platform->DrawSprite(Sprite.Sprite, (f32 *)M.Elements);
  }
  
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}