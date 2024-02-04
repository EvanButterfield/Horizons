#ifndef WIN32_HORIZONS_H
#define WIN32_HORIZONS_H

typedef struct win32_state
{
  input GameInput;
  s64 PerfCountFrequency;
  b32 ShowCursor;
  
  HWND Window;
  
  window_dimension WindowDimension;
  
  memory_arena Arena;
  
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
