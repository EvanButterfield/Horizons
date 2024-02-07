#include "horizons.h"

#include "string.h"

global game_state *State;

GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  State = (game_state *)Memory->PermanentStorage;
  if(!State->Initialized)
  {
    InitializeArena(&State->PermArena, (u8 *)Memory->PermanentStorage + sizeof(game_state),
                    Memory->PermanentStorageSize - sizeof(game_state));
    InitializeArena(&State->TempArena, (u8 *)Memory->TempStorage, Memory->TempStorageSize);
    
    State->Initialized = true;
  }
  
  State->TempArena.Used = 0;
  return(false);
}

// TODO(evan): #if windows
#include <windows.h>
BOOL WINAPI _DllMainCRTStartup(HINSTANCE Instace,
                               DWORD Reason,
                               LPVOID Reserved)
{
  return(TRUE);
}