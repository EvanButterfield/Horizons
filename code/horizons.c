#include "horizons.h"
#include "horizons_string.h"

global game_state *State;
global platform_api *Platform;

#define LetterJustPressed(Input, Letter) ((Input)->Keyboard.Letters[(Letter) - 'A'] == 1 && State->LastInput.Keyboard.Letters[(Letter) - 'A'] == 0)
#define LetterDown(Input, Letter) (Input)->Keyboard.Letters[(Letter) - 'A']

#include "horizons_json.c"
#include "horizons_gltf.c"

internal void
AddCollider(game_aabb Collider)
{
  if(State->NumColliders >= MAX_COLLIDERS)
  {
    Platform->LogMessagePlain("Too many colliders\n", true, MESSAGE_SEVERITY_WARNING);
    return;
  }
  
  State->Colliders[State->NumColliders++] = Collider;
}

internal game_aabb
DrawMesh(game_mesh *Mesh, vec3 Position, vec3 Rotation, vec3 Scale, mat4 PrevM)
{
  mat4 Transform = Mat4CreateTransform3D(Position, Rotation, Scale);
  mat4 M = Mat4Mul(Transform, PrevM);
  
  vs_shader_constants VSConstants;
  VSConstants.Matrix = M;
  VSConstants.Transform = Transform;
  VSConstants.Color = Mesh->Material->Color;
  
  ps_shader_constants PSConstants;
  PSConstants.AmbientStrength = State->AmbientStrength;
  PSConstants.LightDirection = State->LightDirection;
  PSConstants.LightColor = State->LightColor;
  
  Platform->DrawMesh(&VSConstants, &PSConstants);
  
  vec3 *TransCollisionPositions = PushArray(&State->TempArena, vec3, Mesh->CollisionPositionCount);
  for(s32 CollisionPositionIndex = 0;
      CollisionPositionIndex < Mesh->CollisionPositionCount;
      ++CollisionPositionIndex)
  {
    vec3 Pos = Mesh->CollisionPositions[CollisionPositionIndex];
    
    mat3 Transform3 = Mat3FromMat4(&Transform);
    vec3 NewPos = Vec3MulMat3(Pos, &Transform3);
    NewPos = Vec3Add(NewPos, Position);
    TransCollisionPositions[CollisionPositionIndex] = NewPos;
  }
  
  game_aabb Result = {TransCollisionPositions[0], TransCollisionPositions[0]};
  for(s32 CollisionPositionIndex = 1;
      CollisionPositionIndex < Mesh->CollisionPositionCount;
      ++CollisionPositionIndex)
  {
    vec3 Point = TransCollisionPositions[CollisionPositionIndex];
    if(Point.x > Result.Max.x)
    {
      Result.Max.x = Point.x;
    }
    else if(Point.y > Result.Max.y)
    {
      Result.Max.y = Point.y;
    }
    else if(Point.z > Result.Max.z)
    {
      Result.Max.z = Point.z;
    }
    
    if(Point.x < Result.Min.x)
    {
      Result.Min.x = Point.x;
    }
    else if(Point.y < Result.Min.y)
    {
      Result.Min.y = Point.y;
    }
    else if(Point.z < Result.Min.z)
    {
      Result.Min.z = Point.z;
    }
  }
  PopArray(&State->TempArena, vec3, Mesh->CollisionPositionCount);
  
  {
    f32 DistX = Result.Max.x - Result.Min.x;
    f32 DistY = Result.Max.y - Result.Min.y;
    f32 DistZ = Result.Max.z - Result.Min.z;
    Result.MidPoint.x = Result.Min.x + DistX/2;
    Result.MidPoint.y = Result.Min.y + DistY/2;
    Result.MidPoint.z = Result.Min.z + DistZ/2;
  }
  
  return(Result);
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
    
    State->CameraPosition = Vec3(0, 0, 5);
    State->CameraRotation = Vec3(0, 90, 0);
    State->CameraFront = Vec3(0, 0, 0);
    State->CameraUp = Vec3(0, 1, 0);
    State->CameraSpeed = 12;
    State->CameraColliderSize = .2f;
    State->IsColliding = false;
    
    State->AmbientStrength = .1f;
    State->LightDirection = Vec3(-.2f, -1, -.3f);
    State->LightColor = Vec3(.3f, .3f, .3f);
    
    State->Mode = GAME_MODE_VIEW;
    
    LoadGLTF("cube.gltf", &State->CubeMeshes);
    LoadGLTF("cone.gltf", &State->ConeMeshes);
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  State->Time += DeltaTime;
  f32 CameraSpeed = State->CameraSpeed*DeltaTime;
  
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
  
  State->CameraFront = Vec3FPEulerToRotation(State->CameraRotation.x*DEG_TO_RAD,
                                             State->CameraRotation.y*DEG_TO_RAD);
  
  if(State->Mode != GAME_MODE_VIEW)
  {
    f32 Sensitivity = 25;
    f32 DeltaX = (f32)Input->Mouse.X - State->LastInput.Mouse.X;
    f32 DeltaY = (f32)Input->Mouse.Y - State->LastInput.Mouse.Y;
    f32 MouseX = DeltaX*Sensitivity*DeltaTime;
    f32 MouseY = DeltaY*Sensitivity*DeltaTime;
    
    State->CameraRotation.Pitch += MouseY;
    if(State->CameraRotation.Pitch > 89)
    {
      State->CameraRotation.Pitch = 89;
    }
    if(State->CameraRotation.Pitch < -89)
    {
      State->CameraRotation.Pitch = -89;
    }
    
    // TODO(evan): Make sure this is between 0 and 360
    State->CameraRotation.Yaw -= MouseX;
  }
  
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
    Movement = Vec2Scale(Movement, CameraSpeed);
    
    if(State->Mode == GAME_MODE_FIRST_PERSON)
    {
      vec3 CameraGroundedRotation =
        Vec3Normalize(Vec3(State->CameraFront.x, 0, State->CameraFront.z));
      vec3 NewPosition =
        Vec3Add(State->CameraPosition, Vec3Scale(CameraGroundedRotation, Movement.z));
      NewPosition =
        Vec3Add(NewPosition,
                Vec3Scale(Vec3Normalize(Vec3Cross(CameraGroundedRotation,
                                                  State->CameraUp)),
                          Movement.x));
      State->CameraPosition = NewPosition;
    }
  }
  
  if(State->Mode == GAME_MODE_DEV)
  {
    vec3 CrossFrontUp = Vec3Normalize(Vec3Cross(State->CameraFront, State->CameraUp));
    if(LetterDown(Input, 'W'))
    {
      State->CameraPosition = Vec3Subtract(State->CameraPosition, Vec3Scale(State->CameraFront, CameraSpeed));
    }
    if(LetterDown(Input, 'S'))
    {
      State->CameraPosition = Vec3Add(State->CameraPosition, Vec3Scale(State->CameraFront, CameraSpeed));
    }
    if(LetterDown(Input, 'A'))
    {
      State->CameraPosition = Vec3Subtract(State->CameraPosition, Vec3Scale(CrossFrontUp, CameraSpeed)); 
    }
    if(LetterDown(Input, 'D'))
    {
      State->CameraPosition = Vec3Add(State->CameraPosition, Vec3Scale(CrossFrontUp, CameraSpeed));
    }
  }
  
  State->IsColliding = false;
  game_aabb CameraCollider;
  CameraCollider.Max = Vec3AddScalar(State->CameraPosition, State->CameraColliderSize);
  CameraCollider.Min = Vec3SubtractScalar(State->CameraPosition, State->CameraColliderSize);
  CameraCollider.MidPoint = State->CameraPosition;
  for(s32 ColliderIndex = 0;
      ColliderIndex < State->NumColliders;
      ++ColliderIndex)
  {
    game_aabb Collider = State->Colliders[ColliderIndex];
    
    if(CameraCollider.Max.x >= Collider.Min.x &&
       CameraCollider.Min.x <= Collider.Max.x &&
       CameraCollider.Max.y >= Collider.Min.y &&
       CameraCollider.Min.y <= Collider.Max.y &&
       CameraCollider.Max.z >= Collider.Min.z &&
       CameraCollider.Min.z <= Collider.Max.z)
    {
      State->IsColliding = true;
      vec3 RawDist = Vec3Subtract(State->CameraPosition, Collider.MidPoint);
      vec3 Dist = Vec3((f32)fabs(RawDist.x), (f32)fabs(RawDist.y), (f32)fabs(RawDist.z));
      if(Dist.x >= Dist.y && Dist.x >= Dist.z)
      {
        if(RawDist.x < 0)
        {
          // Positive X Coll
          State->CameraPosition.x = Collider.Min.x - State->CameraColliderSize;
        }
        else
        {
          // Negative X Coll
          State->CameraPosition.x = Collider.Max.x + State->CameraColliderSize;
        }
      }
      else if(Dist.y >= Dist.x && Dist.y >= Dist.z)
      {
        if(RawDist.y < 0)
        {
          // Positive Y Coll
          State->CameraPosition.y = Collider.Min.y - State->CameraColliderSize;
        }
        else
        {
          // Negative Y Coll
          State->CameraPosition.y = Collider.Max.y + State->CameraColliderSize;
        }
      }
      else
      {
        if(RawDist.z > 0)
        {
          // Positive Z Coll
          State->CameraPosition.z = Collider.Max.z + State->CameraColliderSize;
        }
        else
        {
          // Negative Z Coll
          State->CameraPosition.z = Collider.Min.z - State->CameraColliderSize;
        }
      }
    }
  }
  
  f32 Aspect = (f32)WindowDimension.Width / WindowDimension.Height;
  mat4 Perspective = Mat4Perspective(Aspect, 60, 0.01f, 100);
  mat4 View = Mat4View(State->CameraPosition, State->CameraFront, State->CameraUp);
  mat4 PrevM = Mat4Mul(View, Perspective);
  
  State->NumColliders = 0;
  
  Platform->SetMesh(State->CubeMeshes[0].Mesh);
  AddCollider(DrawMesh(State->CubeMeshes, Vec3(-1, 0, 0), Vec3(0, 0, 0), Vec3(1, 1.5f, 1), PrevM));
  
  Platform->SetMesh(State->ConeMeshes[0].Mesh);
  AddCollider(DrawMesh(State->ConeMeshes, Vec3(5, 0, 0), Vec3(0, 0, 0), Vec3(1, 1, 1), PrevM));
  
  State->LastInput = *Input;
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}