#include "horizons.h"
#include "horizons_string.h"

global game_state *State;
global platform_api *Platform;

#include "horizons_json.c"
#include "horizons_gltf.c"

#define LetterJustPressed(Input, Letter) ((Input)->Keyboard.Letters[(Letter) - 'A'] == 1 && State->LastInput.Keyboard.Letters[(Letter) - 'A'] == 0)
#define LetterDown(Input, Letter) (Input)->Keyboard.Letters[(Letter) - 'A']

internal s32
CreateEntity(entity **Entity, entity_flags Flags,
             collider_type ColliderType, render_type RenderType)
{
  if(State->EntityCount >= MAX_ENTITIES)
  {
    Platform->LogMessagePlain("Too many entities\n", true, MESSAGE_SEVERITY_WARNING);
  }
  
  entity NewEntity =
  {
    .Flags = Flags,
    .ColliderType = ColliderType,
    .RenderType = RenderType
  };
  s32 Index = State->EntityCount;
  State->Entities[Index] = NewEntity;
  
  if(Entity)
  {
    *Entity = &State->Entities[Index];
  }
  
  ++State->EntityCount;
  return(Index);
}

internal aabb
CreateAABB(vec3 Max, vec3 Min)
{
  aabb Result = {.Max = Max, .Min = Min};
  
  f32 DistX = Result.Max.x - Result.Min.x;
  f32 DistY = Result.Max.y - Result.Min.y;
  f32 DistZ = Result.Max.z - Result.Min.z;
  
  return(Result);
}

internal vec3
AABBMidPoint(aabb AABB)
{
  vec3 Result;
  Result.x = (AABB.Min.x + AABB.Max.x)/2;
  Result.y = (AABB.Min.y + AABB.Max.y)/2;
  Result.z = (AABB.Min.z + AABB.Max.z)/2;
  return(Result);
}

internal vec3
AABBHalfSize(aabb AABB)
{
  vec3 Result;
  Result.x = (AABB.Max.x - AABB.Min.x)/2;
  Result.y = (AABB.Max.y - AABB.Min.y)/2;
  Result.z = (AABB.Max.z - AABB.Min.z)/2;
  return(Result);
}

// TODO(evan): Represent this whole thing in terms of max/min
internal aabb
MinkowskiDifference(aabb A, aabb B)
{
  vec3 PosA = AABBMidPoint(A);
  vec3 HalfSizeA = AABBHalfSize(A);
  vec3 PosB = AABBMidPoint(B);  
  vec3 HalfSizeB = AABBHalfSize(B);
  
  vec3 DP = Vec3Subtract(PosA, PosB);
  vec3 AS = Vec3Add(HalfSizeA, HalfSizeB);
  
  aabb Result;
  Result.Max = Vec3Add(DP, AS);
  Result.Min = Vec3Subtract(DP, AS);
  return(Result);
}

internal void
RenderAABB(aabb AABB, mat4 PrevM,
           ps_shader_constants PSConstants, vec4 Color)
{
  vec3 Position = AABBMidPoint(AABB);
  vec3 Rotation = Vec3(0, 0, 0);
  vec3 Scale = Vec3(AABB.Max.x - AABB.Min.x,
                    AABB.Max.y - AABB.Min.y,
                    AABB.Max.z - AABB.Min.z);
  Scale = Vec3Scale(Scale, 0.5f);
  mat4 Transform = Mat4CreateTransform3D(Position,
                                         Rotation,
                                         Scale);
  mat4 M = Mat4Mul(Transform, PrevM);
        
  vs_shader_constants VSConstants =
  {
    .M = M,
    .Transform = Transform,
    .Color = Color
  };
        
  Platform->DrawMesh(&VSConstants, &PSConstants);
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
    
    State->Time = 0;
    
    State->CubeMeshCount = LoadGLTF("cube.gltf", &State->CubeMeshes);
    State->ConeMeshCount = LoadGLTF("cone.gltf", &State->ConeMeshes);
    
    entity *CameraEntity;
    entity_flags CameraFlags = ENTITY_FLAGS_IS_CAMERA |
                               ENTITY_FLAGS_GENERATE_BASIC_COLLISIONS |
                               ENTITY_FLAGS_MOVING |
                               ENTITY_FLAGS_EXCLUDE_FROM_COLLISION_DRAW;
    State->CurrentCameraIndex = CreateEntity(&CameraEntity, CameraFlags,
                                             COLLIDER_BOX, RENDER_TYPE_NONE);
    CameraEntity->BasicColliderSize = Vec3Scalar(.2f);
    CameraEntity->Speed = 12;
    CameraEntity->Position = Vec3(0, 0, 5);
    CameraEntity->Rotation = Vec3(0, 90, 0);
    CameraEntity->Sensitivity = 25;
    CameraEntity->Near = .1f;
    CameraEntity->Far = 100;
    CameraEntity->FOV = 60;
    CameraEntity->Front = Vec3(0, 0, 0);
    CameraEntity->Up = Vec3(0, 1, 0);
    
    /*entity *Entity2D;
    CreateEntity(&Entity2D, ENTITY_FLAGS_NONE, COLLIDER_NONE, RENDER_TYPE_SPRITE);
    u32 *Texture = PushArray(&State->PermArena, u32, 4);
    Texture[0] = 0xff0000ff;
    Texture[1] = 0xff00ff00;
    Texture[2] = 0xffff0000;
    Texture[3] = 0xff00ffff;
    platform_sprite Sprite = Platform->CreateSprite(Texture, 2, 2);
    Entity2D->Sprite = Sprite;
    Entity2D->Scale = Vec3(100, 100, 1);
    Entity2D->Color = Vec4(1, 1, 1, 1);*/
    
    entity *Entity3D;
    entity_flags Entity3DFlags = ENTITY_FLAGS_GENERATE_MESH_COLLISIONS|
                                 ENTITY_FLAGS_ROTATE;
    CreateEntity(&Entity3D, Entity3DFlags,
                 COLLIDER_BOX, RENDER_TYPE_MESH);
    Entity3D->Meshes = State->CubeMeshes;
    Entity3D->MeshCount = State->CubeMeshCount;
    Entity3D->Scale = Vec3(1, 2, 1);
    
    entity *Entity3D2;
    entity_flags Entity3D2Flags = ENTITY_FLAGS_GENERATE_MESH_COLLISIONS;
    CreateEntity(&Entity3D2, Entity3D2Flags,
                 COLLIDER_BOX, RENDER_TYPE_MESH);
    Entity3D2->Meshes = State->ConeMeshes;
    Entity3D2->MeshCount = State->ConeMeshCount;
    Entity3D2->Position = Vec3(5, 0, 0);
    Entity3D2->Scale = Vec3(1, 1, 1);
    
    State->AmbientStrength = .1f;
    State->LightDirection = Vec3(-.2f, -1, -.3f);
    State->LightColor = Vec3(.3f, .3f, .3f);
    
    State->OutlineShader = Platform->CreateShader("outline_shader");
    State->Shader2d = Platform->CreateShader("2d_shader");
    
    State->Mode = GAME_MODE_VIEW;
    
    {
      State->OutlineMesh.Mesh = Memory->DefaultCubeMesh;
      
      // NOTE: We store the same position 3 times for normal accuracy
      s32 CollisionPositionCount = Memory->DefaultCubeMeshVertexCount/3;
      vec3 *CollisionPositions = PushArray(&State->PermArena, vec3, CollisionPositionCount);
      s32 VerticesScanIndex = 0;
      for(s32 VertexIndex = 0;
          VertexIndex < CollisionPositionCount;
          ++VertexIndex)
      {
        CollisionPositions[VertexIndex] = Memory->DefaultCubeMeshVertices[VerticesScanIndex].Position;
        VerticesScanIndex += 3;
      }
      
      game_material *Material = PushStruct(&State->PermArena, game_material);
      Material->Color = Vec4(0, 1, 0, 1);
      Material->Metallic = 0;
      Material->Roughness = 0;
      
      State->OutlineMesh.CollisionPositions = CollisionPositions;
      State->OutlineMesh.CollisionPositionCount = CollisionPositionCount;
      State->OutlineMesh.Material = Material;
    }
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  State->Time += DeltaTime;
  
  if(LetterJustPressed(Input, 'E'))
  {
    State->Mode = GAME_MODE_VIEW;
  }
  
  if(LetterJustPressed(Input, 'F'))
  {
    State->Mode = GAME_MODE_FIRST_PERSON;
  }
  
  if(LetterJustPressed(Input, 'Q'))
  {
    State->Mode = GAME_MODE_DEV;
  }
  
  f32 MouseDeltaX = (f32)Input->Mouse.X - State->LastInput.Mouse.X;
  f32 MouseDeltaY = (f32)Input->Mouse.Y - State->LastInput.Mouse.Y;
  
  // Entity Update
  for(s32 EntityIndex = 0;
      EntityIndex < State->EntityCount;
      ++EntityIndex)
  {
    entity *Entity = &State->Entities[EntityIndex];
    
    if(Entity->Flags == 0)
    {
      goto FinishEntityUpdate;
    }
    
    // Entity Update Is Camera
    if(Entity->Flags & ENTITY_FLAGS_IS_CAMERA)
    {
      Entity->Front = Vec3FPEulerToRotation(Entity->Rotation.x*DEG_TO_RAD,
                                            Entity->Rotation.y*DEG_TO_RAD);
      
      if(State->Mode == GAME_MODE_FIRST_PERSON ||
         (State->Mode == GAME_MODE_DEV && Input->Mouse.RButton))
      {
        f32 MouseX = MouseDeltaX*Entity->Sensitivity*DeltaTime;
        f32 MouseY = MouseDeltaY*Entity->Sensitivity*DeltaTime;
        
        Entity->Rotation.Pitch += MouseY;
        if(Entity->Rotation.Pitch > 89)
        {
          Entity->Rotation.Pitch = 89;
        }
        if(Entity->Rotation.Pitch < -89)
        {
          Entity->Rotation.Pitch = -89;
        }
        
        // TODO(evan): Make sure this is between 0 and 360
        //             (e.g. 361 == 1, 362 == 2, etc.)
        Entity->Rotation.Yaw -= MouseX;
      }
      
      f32 Speed = Entity->Speed*DeltaTime;
      if(State->Mode == GAME_MODE_FIRST_PERSON)
      {
        vec2 Movement = {0};
        if(LetterDown(Input, 'W'))
        {
          Movement.z -= 1;
        }
        if(LetterDown(Input, 'S'))
        {
          Movement.z += 1;
        }
        if(LetterDown(Input, 'A'))
        {
          Movement.x -= 1;
        }
        if(LetterDown(Input, 'D'))
        {
          Movement.x += 1;
        }
        Movement = Vec2Normalize(Movement);
        Movement = Vec2Scale(Movement, Speed);
        
        vec3 CameraGroundedRotation =
          Vec3Normalize(Vec3(Entity->Front.x, 0, Entity->Front.z));
        vec3 NewPosition =
          Vec3Add(Entity->Position, Vec3Scale(CameraGroundedRotation, Movement.z));
        NewPosition =
          Vec3Add(NewPosition,
                  Vec3Scale(Vec3Normalize(Vec3Cross(CameraGroundedRotation,
                                                    Entity->Up)),
                            Movement.x));
        Entity->Position = NewPosition;
      }
      else if(State->Mode == GAME_MODE_DEV)
      {
        vec3 CrossFrontUp = Vec3Normalize(Vec3Cross(Entity->Front, Entity->Up));
        if(LetterDown(Input, 'W'))
        {
          Entity->Position = Vec3Subtract(Entity->Position, Vec3Scale(Entity->Front, Speed));
        }
        if(LetterDown(Input, 'S'))
        {
          Entity->Position = Vec3Add(Entity->Position, Vec3Scale(Entity->Front, Speed));
        }
        if(LetterDown(Input, 'A'))
        {
          Entity->Position = Vec3Subtract(Entity->Position, Vec3Scale(CrossFrontUp, Speed)); 
        }
        if(LetterDown(Input, 'D'))
        {
          Entity->Position = Vec3Add(Entity->Position, Vec3Scale(CrossFrontUp, Speed));
        }
      }
    }
    // END Entity Update Is Camera
    
    // Entity Update Rotate
    if(Entity->Flags & ENTITY_FLAGS_ROTATE)
    {
      Entity->Rotation = Vec3AddScalar(Entity->Rotation, DeltaTime*10);
    }
    // END Entity Update Rotate
    
    FinishEntityUpdate:
    Entity->NumCollisions = 0;
  }
  // END Entity Update
  
  entity *CurrentCamera = &State->Entities[State->CurrentCameraIndex];
  
  f32 Aspect = (f32)WindowDimension.Width / WindowDimension.Height;
  mat4 Perspective = Mat4Perspective(Aspect, CurrentCamera->FOV,
                                     CurrentCamera->Near,
                                     CurrentCamera->Far);
  mat4 View = Mat4View(CurrentCamera->Position,
                       CurrentCamera->Front,
                       CurrentCamera->Up);
  mat4 PrevM = Mat4Mul(View, Perspective);
  
  mat4 Ortho = Mat4Orthographic(-1, 1, 1, -1,
                                CurrentCamera->Near, CurrentCamera->Far);
  
  ps_shader_constants PSConstants;
  PSConstants.AmbientStrength = State->AmbientStrength;
  PSConstants.LightDirection = State->LightDirection;
  PSConstants.LightColor = State->LightColor;
  PSConstants.CameraPosition = CurrentCamera->Position;
  
  // Collision System
  // TODO(evan): This only supports static vs static detection and
  //             moving vs static detection
  for(s32 EntityIndex = 0;
      EntityIndex < State->EntityCount;
      ++EntityIndex)
  {
    entity *Entity = &State->Entities[EntityIndex];
    
    if(Entity->ColliderType == COLLIDER_NONE)
    {
      continue;
    }
     
    for(s32 SecondEntityIndex = 0;
        SecondEntityIndex < State->EntityCount;
        ++SecondEntityIndex)
    {
      if(EntityIndex == SecondEntityIndex)
      {
        continue;
      }
      
      entity *SecondEntity = &State->Entities[SecondEntityIndex];
      
      if(SecondEntity->ColliderType == COLLIDER_NONE)
      {
        continue;
      }
      
      // Collision System Box-Box
      if(Entity->ColliderType == COLLIDER_BOX &&
         SecondEntity->ColliderType == COLLIDER_BOX)
      {
        aabb Collider = Entity->AABB;
        aabb SecondCollider = SecondEntity->AABB;
        
        aabb Difference = MinkowskiDifference(Collider, SecondCollider);
        Platform->SetRenderMode(PLATFORM_RENDER_WIREFRAME);
        Platform->SetShader(State->OutlineShader);
        Platform->SetMesh(State->CubeMeshes[0].Mesh);
        // RenderAABB(Difference, PrevM, PSConstants, Vec4(1, 0, 0, 1));
        if(Difference.Min.x <= 0 && Difference.Max.x >= 0 &&
           Difference.Min.y <= 0 && Difference.Max.y >= 0 &&
           Difference.Min.z <= 0 && Difference.Max.z >= 0)
        {
          Entity->OtherCollisionIndices[Entity->NumCollisions++] = SecondEntityIndex;
        
          if(Entity->Flags & ENTITY_FLAGS_MOVING)
          {
            f32 MinDist = fabsf(Difference.Min.x);
            vec3 Penetration = Vec3(Difference.Min.x, 0, 0);
            
            if(fabsf(Difference.Max.x) < MinDist)
            {
              MinDist = fabsf(Difference.Max.x);
              Penetration.x = Difference.Max.x;
            }
            
            if(fabsf(Difference.Min.y) < MinDist)
            {
              MinDist = fabsf(Difference.Min.y);
              Penetration.x = 0;
              Penetration.y = Difference.Min.y;
            }
            
            if(fabsf(Difference.Max.y) < MinDist)
            {
              MinDist = fabsf(Difference.Max.y);
              Penetration.x = 0;
              Penetration.y = Difference.Max.y;
            }
            
            if(fabsf(Difference.Min.z) < MinDist)
            {
              MinDist = fabsf(Difference.Min.z);
              Penetration.x = 0;
              Penetration.y = 0;
              Penetration.z = Difference.Min.z;
            }
            
            if(fabsf(Difference.Max.x) < MinDist)
            {
              Penetration.x = 0;
              Penetration.y = 0;
              Penetration.z = Difference.Max.z;
            }
            
            Entity->Position = Vec3Subtract(Entity->Position, Penetration);
          }
        }
      }
      // END Collision System Box-Box
    }
  }
  // END Collision System
  
  // Render System
  for(s32 EntityIndex = 0;
      EntityIndex < State->EntityCount;
      ++EntityIndex)
  {
    entity *Entity = &State->Entities[EntityIndex];
    
    if(Entity->Flags & ENTITY_FLAGS_GENERATE_BASIC_COLLISIONS)
    {
      if(Entity->ColliderType == COLLIDER_BOX)
      {
        vec3 Max = Vec3Add(Entity->Position, Entity->BasicColliderSize);
        vec3 Min = Vec3Subtract(Entity->Position, Entity->BasicColliderSize);
        Entity->AABB = CreateAABB(Max, Min);
      }
    }
    
    switch(Entity->RenderType)
    {
      case RENDER_TYPE_NONE:
      {
        continue;
      } break;
      
      case RENDER_TYPE_SPRITE:
      {
        if(Entity->Sprite == 0)
        {
          Platform->LogMessagePlain("Tried to render a sprite entity without a sprite set\n",
                                    true, MESSAGE_SEVERITY_WARNING);
          continue;
        }
        Platform->SetSprite(Entity->Sprite);
        
        if(Entity->Shader)
        {
          Platform->SetShader(Entity->Shader);
        }
        else
        {
          Platform->SetShader(State->Shader2d);
        }

        mat4 Transform = Mat4CreateTransform2D(Entity->Position,
                                               Entity->Rotation,
                                               Entity->Scale,
                                               WindowDimension);
        mat4 M = Mat4Mul(Transform, Ortho);
        vs_shader_constants_2d Constants =
        {
          .M = M,
          .Color = Entity->Color
        };

        Platform->SetRenderMode(PLATFORM_RENDER_2D);        
        Platform->DrawSprite(&Constants);
      } break;
      
      case RENDER_TYPE_MESH:
      {
        if(Entity->Sprite)
        {
          Platform->SetSprite(Entity->Sprite);
        }
        else
        {
          Platform->SetSprite(Memory->DefaultSprite);
        }
      
        if(Entity->Shader)
        {
          Platform->SetShader(Entity->Shader);
        }
        else
        {
          Platform->SetShader(Memory->DefaultShader);
        }
        
        Platform->SetRenderMode(PLATFORM_RENDER_SOLID);
        
        mat4 Transform = Mat4CreateTransform3D(Entity->Position,
                                               Entity->Rotation,
                                               Entity->Scale);
        mat4 M = Mat4Mul(Transform, PrevM);
        
        vs_shader_constants VSConstants;
        VSConstants.M = M;
        VSConstants.Transform = Transform;
        
        for(s32 MeshIndex = 0;
            MeshIndex < Entity->MeshCount;
            ++MeshIndex)
        {
          game_mesh *Mesh = &Entity->Meshes[MeshIndex];
          if(Mesh->Mesh == 0)
          {
            Platform->LogMessagePlain("Tried to render a mesh entity without on of the Meshes set\n",
                                      true, MESSAGE_SEVERITY_WARNING);
            continue;
          }
          Platform->SetMesh(Mesh->Mesh);
        
          VSConstants.Color = Mesh->Material->Color;
          Platform->DrawMesh(&VSConstants, &PSConstants);
          
          // NOTE: Only generate collisions from render data for 3D meshes
          //       because only 3D meshes will need collisions
          // Render System Generate Mesh Collider
          if(Entity->Flags & ENTITY_FLAGS_GENERATE_MESH_COLLISIONS)
          {
            // Render System Generate Mesh Collider Box
            if(Entity->ColliderType == COLLIDER_BOX)
            {
              vec3 *TransCollisionPositions = PushArray(&State->TempArena, vec3,
                                                        Mesh->CollisionPositionCount);
              for(s32 CollisionPositionIndex = 0;
                  CollisionPositionIndex < Mesh->CollisionPositionCount;
                  ++CollisionPositionIndex)
              {
                vec3 Pos = Mesh->CollisionPositions[CollisionPositionIndex];
                
                mat4 TransposeTransform = Mat4Transpose(&Transform);
                vec4 NewPos = Vec4MulMat4(Vec4FromVec3(Pos, 1), &TransposeTransform);
                TransCollisionPositions[CollisionPositionIndex] = NewPos.xyz;
              }
              
              vec3 Max = TransCollisionPositions[0];
              vec3 Min = Max;
              for(s32 CollisionPositionIndex = 1;
                  CollisionPositionIndex < Mesh->CollisionPositionCount;
                  ++CollisionPositionIndex)
              {
                vec3 Point = TransCollisionPositions[CollisionPositionIndex];
                if(Point.x > Max.x)
                {
                  Max.x = Point.x;
                }
                if(Point.y > Max.y)
                {
                  Max.y = Point.y;
                }
                if(Point.z > Max.z)
                {
                  Max.z = Point.z;
                }
                
                if(Point.x < Min.x)
                {
                  Min.x = Point.x;
                }
                if(Point.y < Min.y)
                {
                  Min.y = Point.y;
                }
                if(Point.z < Min.z)
                {
                  Min.z = Point.z;
                }
              }
              PopArray(&State->TempArena, vec3, Mesh->CollisionPositionCount);
              
              Entity->AABB = CreateAABB(Max, Min);
            }
            // END Render System Generate Mesh Collider Box
          }
          // END Render System Generate Mesh Collider
        }
      } break;
    }
  }
  // END Render System
  
  if(State->Mode == GAME_MODE_DEV)
  {
    Platform->SetRenderMode(PLATFORM_RENDER_WIREFRAME);
    Platform->SetShader(State->OutlineShader);
    Platform->SetMesh(State->CubeMeshes[0].Mesh);
  
    for(s32 EntityIndex = 0;
        EntityIndex < State->EntityCount;
        ++EntityIndex)
    {
      entity *Entity = &State->Entities[EntityIndex];
      
      if(Entity->Flags & ENTITY_FLAGS_EXCLUDE_FROM_COLLISION_DRAW)
      {
        continue;
      }
      
      if(Entity->ColliderType == COLLIDER_BOX)
      {
        aabb AABB = Entity->AABB;
        #if 0
        vec3 Position = AABBMidPoint(AABB);
        vec3 Rotation = Vec3(0, 0, 0);
        vec3 Scale = Vec3(AABB.Max.x - AABB.Min.x,
                          AABB.Max.y - AABB.Min.y,
                          AABB.Max.z - AABB.Min.z);
        Scale = Vec3Scale(Scale, 0.5f);
        mat4 Transform = Mat4CreateTransform3D(Position,
                                               Rotation,
                                               Scale);
        mat4 M = Mat4Mul(Transform, PrevM);
        
        vs_shader_constants VSConstants =
        {
          .M = M,
          .Transform = Transform,
          .Color = Vec4(0, 1, 0, 1)
        };
        
        Platform->DrawMesh(&VSConstants, &PSConstants);
        #endif
        
        RenderAABB(AABB, PrevM, PSConstants, Vec4(0, 1, 0, 1));
      }
    }
  }
  
  State->LastInput = *Input;
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}