int _fltused;

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
    State->CosAngle = 1;
    State->CosUp = false;
    State->SinAngle = 0;
    State->SinUp = true;
    
    State->Initialized = true;
  }
  
  if(State->CosUp)
  {
    State->CosAngle += DeltaTime;
    if(State->CosAngle >= 1)
    {
      State->CosAngle = 1;
      State->CosUp = false;
    }
  }
  else
  {
    State->CosAngle -= DeltaTime;
    if(State->CosAngle <= -1)
    {
      State->CosAngle = -1;
      State->CosUp = true;
    }
  }
  
  if(State->SinUp)
  {
    State->SinAngle += DeltaTime;
    if(State->SinAngle >= 1)
    {
      State->SinAngle = 1;
      State->SinUp = false;
    }
  }
  else
  {
    State->SinAngle -= DeltaTime;
    if(State->SinAngle <= -1)
    {
      State->SinAngle = -1;
      State->SinUp = true;
    }
  }
  
  Platform->DrawSprite(State->Sprite, 0.5f, 0.5f);
  Platform->DrawSprite(State->Sprite, State->CosAngle, State->SinAngle);
  Platform->DrawSprite(State->Sprite, 1, 0);
  
  State->TempArena.Used = 0;
  return(false);
}

#if OS_WIN
#include <windows.h>
BOOL WINAPI _DllMainCRTStartup(HINSTANCE Instace,
                               DWORD Reason,
                               LPVOID Reserved)
{
  return(TRUE);
}
#endif