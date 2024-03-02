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
    
    {
      game_entity Entity =
      {
        .Sprite = SPRITES_SPRITE,
        .IsPhysicsBody = true,
        .IsCollidable = true,
        .AffectedByGravity = true
      };
      SpriteEntity = CreateEntity(Entity);
    }
    
    {
      State->StartChunkEntityIndex = State->EntityCount;
      f32 ChunkX = 0;
      for(s32 ChunkIndex = 0; ChunkIndex < NumWorldChunks; ++ChunkIndex)
      {
        world_chunk Chunk = WorldChunks[ChunkIndex];
        vec2 HalfSize = State->Sprites[Chunk.SpriteIndex].HalfSize;
        game_entity Entity =
        {
          .Sprite = Chunk.SpriteIndex,
          .Pos.x = ChunkX,
          .Pos.y = Chunk.Y + HalfSize.y,
          .IsCollidable = true
        };
        
        ChunkX += State->Sprites[SPRITES_GROUND].Size.x;
        
        CreateEntity(Entity);
      }
    }
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  // State->Entities[SpriteEntity].Angle += DeltaTime;
  
  f32 Aspect = (f32)WindowDimension.Height / WindowDimension.Width;
  mat4 Orth = Mat4Orthographic(0, (f32)WindowDimension.Width,
                               (f32)WindowDimension.Height, 0,
                               0, 10);
  
  /*{
    game_entity *Entity = &State->Entities[SpriteEntity];
    Entity->Pos.x = (f32)Input->Mouse.X - WindowDimension.Width/2;
    Entity->Pos.y = (f32)Input->Mouse.Y - WindowDimension.Height/2;
  }*/
  {
    f32 Movement = 0;
    if(Input->Keyboard.Right)
    {
      Movement += 1;
    }
    if(Input->Keyboard.Left)
    {
      Movement -= 1;
    }
    
    State->Entities[SpriteEntity].Velocity.x += Movement*DeltaTime;
  }
  
  for(s32 EntityIndex = 0; EntityIndex < State->EntityCount; ++EntityIndex)
  {
    game_entity *Entity = &State->Entities[EntityIndex];
    if(Entity->IsPhysicsBody)
    {
      UpdatePhysics(EntityIndex, DeltaTime);
    }
    
    game_sprite Sprite = State->Sprites[Entity->Sprite];
    vec2 Pos = Entity->Pos;
    
    mat4 Transform = Mat4CreateTransform(false, Pos, Entity->Angle, Sprite.Size, WindowDimension);
    mat4 M = Mat4Mul(Transform, Orth);
    Platform->DrawSprite(Sprite.Sprite, (f32 *)M.Elements);
  }
  
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}

internal void
UpdatePhysics(s32 EntityIndex, f32 DeltaTime)
{
  game_entity *Entity = &State->Entities[EntityIndex];
  
  // DeltaVY = gt
  if(Entity->AffectedByGravity)
  {
    f32 DeltaVY = GAcceleration*DeltaTime;
    Entity->Velocity.y += DeltaVY;
  }
  
  Entity->Pos = Vec2Add(Entity->Pos, Entity->Velocity);
  
  game_sprite Sprite = State->Sprites[Entity->Sprite];
  f32 Left1 = Entity->Pos.x - Sprite.HalfSize.x;
  f32 Right1 = Entity->Pos.x + Sprite.HalfSize.x;
  f32 Top1 = Entity->Pos.y - Sprite.HalfSize.y;
  f32 Bottom1 = Entity->Pos.y + Sprite.HalfSize.y;
  for(s32 OtherIndex = 0; OtherIndex < State->EntityCount; ++OtherIndex)
  {
    if(EntityIndex == OtherIndex)
    {
      continue;
    }
    
    game_entity Other = State->Entities[OtherIndex];
    game_sprite OtherSprite = State->Sprites[Other.Sprite];
    f32 Left2 = Other.Pos.x - OtherSprite.HalfSize.x;
    f32 Right2 = Other.Pos.x + OtherSprite.HalfSize.x;
    f32 Top2 = Other.Pos.y - OtherSprite.HalfSize.y;
    f32 Bottom2 = Other.Pos.y + OtherSprite.HalfSize.y;
    
    game_direction Direction;
    if(Right1 >= Left2 &&
       Left1 <= Right2 &&
       Top1 <= Bottom2 &&
       Bottom1 >= Top2)
    {
      f32 DX = Abs(Entity->Pos.x - Other.Pos.x);
      f32 DY = Abs(Entity->Pos.y - Other.Pos.y);
      
      f32 DistToMoveX;
      if(Entity->Pos.x < Other.Pos.x)
      {
        // Left
        DistToMoveX = -Abs(Right1 - Left2);
      }
      else
      {
        // Right
        DistToMoveX = Abs(Right2 - Left1);
      }
      
      f32 DistToMoveY;
      if(Entity->Pos.y < Other.Pos.y)
      {
        // Top
        DistToMoveY = -Abs(Bottom1 - Top2);
      }
      else
      {
        // Bottom
        DistToMoveY = Abs(Bottom2 - Top1);
      }
      
      if(Abs(DistToMoveX) < Abs(DistToMoveY))
      {
        Direction = (DistToMoveX >= 0) ? DIRECTION_RIGHT : DIRECTION_LEFT;
      }
      else
      {
        Direction = (DistToMoveY >= 0) ? DIRECTION_DOWN : DIRECTION_UP;
      }
    }
    else
    {
      Direction = DIRECTION_NONE;
      Platform->LogMessagePlain("\n", true, MESSAGE_SEVERITY_DEBUG);
    }
    
    if(Direction & (DIRECTION_RIGHT|DIRECTION_LEFT))
    {
      Entity->Velocity.x *= -0.5f;
      
      if(Direction == DIRECTION_RIGHT)
      {
        Entity->Pos.x = Right2 + Sprite.HalfSize.x;
      }
      else
      {
        Entity->Pos.x = Left2 - Sprite.HalfSize.x;
      }
    }
    else if(Direction & (DIRECTION_DOWN|DIRECTION_UP))
    {
      Entity->Velocity.y *= -0.5f;
      
      // NOTE(evan): Friction
      Entity->Velocity.x -= Entity->Velocity.x*0.002f;
      
      if(Direction == DIRECTION_DOWN)
      {
        Entity->Pos.y = Bottom2 + Sprite.HalfSize.y;
      }
      else
      {
        Entity->Pos.y = Top2 - Sprite.HalfSize.x;
      }
    }
  }
}