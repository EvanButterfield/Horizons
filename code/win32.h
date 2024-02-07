#ifndef WIN32_HORIZONS_H
#define WIN32_HORIZONS_H

#define AssertHR(HR) Assert(SUCCEEDED(HR))

#include "d3d11.h"

typedef struct win32_state
{
  game_input GameInput;
  s64 PerfCountFrequency;
  b32 ShowCursor;
  
  HWND Window;
  d3d11_state D3D11State;
  
  window_dimension WindowDimension;
  
  memory_arena PermArena;
  memory_arena TempArena;
  
  b32 WindowClosed;
} win32_state;

typedef struct win32_game_code
{
  b32 IsValid;
  HMODULE GameDll;
  FILETIME LastWriteTime;
  game_update_and_render *GameUpdateAndRender;
} win32_game_code;

#endif //WIN32_HORIZONS_H
