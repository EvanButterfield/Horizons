int _fltused;

#include "types.h"
#include "platform.h"
#include "string.h"

#define COBJMACROS
#include <windows.h>
#include <windowsx.h>
#include <xinput.h>
#include <shlwapi.h>

#undef CopyMemory
#undef ZeroMemory
#undef StrToInt

#include "win32.h"

internal PLATFORM_COPY_MEMORY(Win32CopyMemory)
{
  RtlCopyMemory(Dest, Source, Length);
}

internal PLATFORM_ZERO_MEMORY(Win32ZeroMemory)
{
  RtlZeroMemory(Dest, Length);
}

internal inline window_dimension
Win32GetWindowDimension(HWND Window)
{
  window_dimension Dimension = {0};
  
  RECT ClientRect;
  GetClientRect(Window, &ClientRect);
  Dimension.Width = ClientRect.right - ClientRect.left;
  Dimension.Height = ClientRect.bottom - ClientRect.top;
  return(Dimension);
}

global win32_state *GlobalState;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
  return(ERROR_DEVICE_NOT_CONNECTED);
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
  return(ERROR_DEVICE_NOT_CONNECTED);
}
global x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32OutputString(s8 *Str)
{
  OutputDebugStringA(Str);
}

internal PLATFORM_LOG_MESSAGE(Win32LogMessage)
{
  string8 From;
  if(FromEngine)
  {
    From =
      CreateString("[ENGINE]: ", &GlobalState->TempArena, &GlobalState->Platform);
  }
  else
  {
    From = CreateString("[PLATFORM (WIN32)]: ", &GlobalState->TempArena,
                        &GlobalState->Platform);
  }
  
  string8 Header =
    CatStrings(From, SeverityMessages[Severity], &GlobalState->TempArena,
               &GlobalState->Platform);
  string8 Final = CatStrings(Header, Message, &GlobalState->TempArena,
                             &GlobalState->Platform);
  Final.Str[Final.Length] = '\0';
  Win32OutputString(Final.Str);
}

internal PLATFORM_LOG_MESSAGE_PLAIN(Win32LogMessagePlain)
{
  Win32LogMessage(CreateString(Message, &GlobalState->TempArena,
                               &GlobalState->Platform),
                  FromEngine, Severity);
}

internal void
Win32LoadXInput(void)
{
  HMODULE XInputLibrary = LoadLibraryA("Xinput9_1_0.dll");
  if(!XInputLibrary)
  {
    XInputLibrary = LoadLibraryA("Xinput1_4.dll");
    if(!XInputLibrary)
    {
      XInputLibrary = LoadLibraryA("Xinput1_3.dll");
    }
  }
  
  if(XInputLibrary)
  {
    XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
    XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
  }
}

internal inline FILETIME
Win32GetLastWriteTime(s8 *FileName)
{
  FILETIME LastWriteTime = {0};
  
  WIN32_FILE_ATTRIBUTE_DATA Data;
  if(GetFileAttributesExA(FileName, GetFileExInfoStandard, &Data))
  {
    LastWriteTime = Data.ftLastWriteTime;
  }
  
  return(LastWriteTime);
}

internal win32_game_code
Win32LoadGameCode(s8 *SourceDllName, s8 *TempDllName, s8 *LockName)
{
  win32_game_code Result = {0};
  
  Result.LastWriteTime = Win32GetLastWriteTime(SourceDllName);
  CopyFileA(SourceDllName, TempDllName, false);
  
  WIN32_FILE_ATTRIBUTE_DATA Ignored;
  if(!GetFileAttributesExA(LockName, GetFileExInfoStandard, &Ignored))
  {
    Result.GameDll = LoadLibraryA(TempDllName);
    DWORD Error = GetLastError();
    if(Result.GameDll)
    {
      Result.GameUpdateAndRender =
      (game_update_and_render *)GetProcAddress(Result.GameDll, "GameUpdateAndRender");
      
      Result.IsValid = Result.GameUpdateAndRender != 0;
    }
  }
  
  if(!Result.IsValid)
  {
    Result.GameUpdateAndRender = GameUpdateAndRenderStub;
    Result.LastWriteTime = (FILETIME){0};
  }
  
  return(Result);
}

internal void
Win32UnloadGameCode(win32_game_code *GameCode)
{
  if(GameCode->GameDll)
  {
    FreeLibrary(GameCode->GameDll);
    GameCode->GameDll = 0;
  }
  
  GameCode->IsValid = false;
  GameCode->GameUpdateAndRender = GameUpdateAndRenderStub;
}

internal PLATFORM_OPEN_FILE(Win32OpenFile)
{
  DWORD AccessFlags = 0;
  DWORD ShareMode = 0;
  if(Flags & FILE_OPEN_READ)
  {
    AccessFlags |= GENERIC_READ;
    ShareMode |= FILE_SHARE_READ;
  }
  if(Flags & FILE_OPEN_WRITE)
  {
    AccessFlags |= GENERIC_WRITE;
    ShareMode |= FILE_SHARE_WRITE;
  }
  
  string8 FullFileName;
  if(IsResource)
  {
    FullFileName = CatStrings(ResourcePath, FileName, &GlobalState->TempArena, &GlobalState->Platform);
  }
  else
  {
    FullFileName = FileName;
  }
  
  HANDLE File = CreateFileA(FullFileName.Str, AccessFlags, ShareMode, 0,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  
  platform_file_handle Result;
  Result.Handle = File;
  Result.FileName = DuplicateString(FullFileName, &GlobalState->PermArena,
                                    &GlobalState->Platform);
  return(Result);
}

internal PLATFORM_GET_FILE_SIZE(Win32GetFileSize)
{
  u32 Result = 0;
  
  if(Handle.Handle != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER FileSize;
    if(GetFileSizeEx(Handle.Handle, &FileSize))
    {
      Result = SafeTruncateUInt64(FileSize.QuadPart);
    }
    else
    {
      string8 Message = CatStringsPlain(Handle.FileName, ": could not GetFileSize in Win32GetFileSize\n",
                                        &GlobalState->TempArena, &GlobalState->Platform);
      Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
    }
  }
  else
  {
    string8 Message =
      CatStringsPlain(Handle.FileName, ": did not have a valid handle when being passed to Win32GetFileSize\n",
                      &GlobalState->TempArena, &GlobalState->Platform);
    Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
  }
  
  return(Result);
}

internal PLATFORM_READ_ENTIRE_FILE(Win32ReadEntireFile)
{
  if(Handle.Handle != INVALID_HANDLE_VALUE)
  {
    if(Dest)
    {
      DWORD BytesRead;
      if(ReadFile(Handle.Handle, Dest, FileSize, &BytesRead, 0) &&
         BytesRead == FileSize)
      {
      }
      else
      {
        string8 Message = CatStringsPlain(Handle.FileName, ": failed to ReadFile in Win32ReadEntireFile\n",
                                          &GlobalState->TempArena, &GlobalState->Platform);
        Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
      }
    }
    else
    {
      string8 Message =
        CatStringsPlain(Handle.FileName, ": the \'Dest\' passed to Win32ReadEntireFile was invalid\n",
                        &GlobalState->TempArena, &GlobalState->Platform);
      Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
    }
  }
  else
  {
    string8 Message =
      CatStringsPlain(Handle.FileName, ": did not have a valid handle when being passed to Win32ReadEntireFile\n",
                      &GlobalState->TempArena, &GlobalState->Platform);
    Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
  }
}

internal PLATFORM_WRITE_ENTIRE_FILE(Win32WriteEntireFile)
{
  b32 Result = false;
  
  HANDLE File = *((HANDLE *)Handle.Handle);
  if(File != INVALID_HANDLE_VALUE)
  {
    DWORD BytesWritten;
    if(WriteFile(File, Data, DataSize, &BytesWritten, 0) &&
       BytesWritten == DataSize)
    {
      Result = true;
    }
    else
    {
      string8 Message = CatStringsPlain(Handle.FileName, ": failed to WriteFile in Win32WriteEntireFile\n",
                                        &GlobalState->TempArena, &GlobalState->Platform);
      Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
    }
  }
  else
  {
    string8 Message =
      CatStringsPlain(Handle.FileName, ": did not have a valid handle when being passed to Win32WriteEntireFile\n",
                      &GlobalState->TempArena, &GlobalState->Platform);
    Win32LogMessage(Message, false, MESSAGE_SEVERITY_WARNING);
  }
  
  return(Result);
}

internal PLATFORM_CLOSE_FILE(Win32CloseFile)
{
  CloseHandle(Handle.Handle);
}

internal PLATFORM_SLEEP(Win32Sleep)
{
  Sleep(MS);
}

internal PLATFORM_STR_TO_INT(Win32StrToInt)
{
  s32 Result = StrToIntA(Str);
  return(Result);
}

internal inline LARGE_INTEGER
Win32GetWallClock(void)
{
  LARGE_INTEGER Result;
  QueryPerformanceCounter(&Result);
  return(Result);
}

internal inline f32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
  f32 Result = ((f32)(End.QuadPart - Start.QuadPart) /
                (f32)GlobalState->PerfCountFrequency);
  return(Result);
}

#include "d3d11.c"

internal PLATFORM_CREATE_SPRITE(Win32CreateSprite)
{
  if(GlobalState->D3D11State.Device)
  {
    if(GlobalState->NumSprites < MAX_ENTITIES)
    {
      GlobalState->Sprites[GlobalState->NumSprites] =
        D3D11CreateSprite(&GlobalState->D3D11State, Texture, TexWidth, TexHeight);
      ++GlobalState->NumSprites;
    }
    else
    {
      Win32LogMessagePlain("Already at max sprite capacity\n",
                           false, MESSAGE_SEVERITY_WARNING);
      return(-1);
    }
  }
  else
  {
    Win32LogMessagePlain("D3D11 has not been initialized yet\n",
                         false, MESSAGE_SEVERITY_WARNING);
    return(-1);
  }
  
  return(GlobalState->NumSprites - 1);
}

internal PLATFORM_DRAW_SPRITE(Win32DrawSprite)
{
  d3d11_sprite *Sprite = &GlobalState->Sprites[SpriteIndex];
  D3D11DrawSprite(&GlobalState->D3D11State, Sprite, Matrix,
                  GlobalState->WindowDimension, &GlobalState->Platform);
}

internal LRESULT CALLBACK
Win32WindowProc(HWND Window,
                UINT Message,
                WPARAM WParam,
                LPARAM LParam);

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int CmdShow)
{
  // Assert((uint64)(*((int64*)__readgsqword(0x60) + 0x23)) >= 10);
  
  memory GameMemory = {0};
  {
#if HORIZONS_INTERNAL
    LPVOID BaseAddress = (LPVOID)Tebibytes(2);
#else
    LPVOID BaseAddress = 0;
#endif
    
    GameMemory.PermanentStorageSize = Mebibytes(64);
    GameMemory.TempStorageSize = Gibibytes(1);
    
    platform_api Platform;
    Platform.OpenFile = Win32OpenFile;
    Platform.GetFileSize = Win32GetFileSize;
    Platform.ReadEntireFile = Win32ReadEntireFile;
    Platform.WriteEntireFile = Win32WriteEntireFile;
    Platform.CloseFile = Win32CloseFile;
    Platform.LogMessage = Win32LogMessage;
    Platform.LogMessagePlain = Win32LogMessagePlain;
    
    Platform.CopyMemory = Win32CopyMemory;
    Platform.ZeroMemory = Win32ZeroMemory;
    
    Platform.Sleep = Win32Sleep;
    Platform.StrToInt = Win32StrToInt;
    
    Platform.CreateSprite = Win32CreateSprite;
    Platform.DrawSprite = Win32DrawSprite;
    GameMemory.Platform = Platform;
    
    memory_index PlatformPermMemorySize = Mebibytes(64);
    memory_index PlatformTempMemorySize = Mebibytes(64);
    memory_index PlatformMemorySize = PlatformPermMemorySize + PlatformTempMemorySize;
    
    memory_index TotalGameMemorySize = (GameMemory.PermanentStorageSize +
                                        GameMemory.TempStorageSize +
                                        PlatformMemorySize);
    GlobalState = VirtualAlloc(BaseAddress, sizeof(win32_state) + TotalGameMemorySize,
                               MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    GlobalState->Platform = Platform;
    GlobalState->NumSprites = 0;
    
    InitializeArena(&GlobalState->PermArena,
                    (u8 *)GlobalState + sizeof(win32_state),
                    PlatformPermMemorySize);
    InitializeArena(&GlobalState->TempArena,
                    GlobalState->PermArena.Memory + PlatformPermMemorySize,
                    PlatformTempMemorySize);
    GameMemory.PermanentStorage = GlobalState->TempArena.Memory + PlatformTempMemorySize;
    GameMemory.TempStorage = (u8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;
  }
  
  if(GameMemory.PermanentStorage)
  {
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalState->PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    // NOTE(evan): Set the Windows schedular granularity to 1ms
    // so that Sleep() can be more granular
    UINT DesiredSchedularMS = 1;
    b32 SleepIsGranular = (timeBeginPeriod(DesiredSchedularMS) == TIMERR_NOERROR);
    
    Win32LoadXInput();
    
    WNDCLASSW WindowClass = {0};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = Win32WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursorW(0, IDC_ARROW);
    WindowClass.lpszClassName = L"WindowClass";
    
    s8 *GameCodeDllName = "horizons.dll";
    s8 *GameCodeTempDllName = "horizons_temp.dll";
    s8 *GameCodeLockName = "lock.tmp";
    
    if(RegisterClassW(&WindowClass))
    {
      GlobalState->Window = CreateWindowW(WindowClass.lpszClassName, L"Horizons",
                                          WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                          CW_USEDEFAULT, CW_USEDEFAULT, 1400, 700,
                                          0, 0, Instance, 0);
      
      GlobalState->ShowCursor = true;
      
      if(GlobalState->Window)
      {
        HDC DeviceContext = GetDC(GlobalState->Window);
        
        s32 MonitorRefreshHz = 60;
        s32 Win32RefreshRate = GetDeviceCaps(DeviceContext, VREFRESH);
        if(Win32RefreshRate > 1)
        {
          MonitorRefreshHz = Win32RefreshRate;
        }
        f32 GameUpdateHz = ((f32)MonitorRefreshHz);
        f32 TargetSecondsPerFrame = 1.0f / (f32)GameUpdateHz;
        
        win32_game_code Game = Win32LoadGameCode(GameCodeDllName, GameCodeTempDllName, GameCodeLockName);
        
        GlobalState->WindowDimension = Win32GetWindowDimension(GlobalState->Window);
        
        GlobalState->D3D11State =
          InitD3D11(GlobalState->Window, &GlobalState->Platform);
        
        LARGE_INTEGER LastCounter = Win32GetWallClock();
        b32 ShouldClose = false;
        while(!ShouldClose)
        {
          f32 DeltaTime;
          {
            LARGE_INTEGER EndCounter = Win32GetWallClock();
            DeltaTime = Win32GetSecondsElapsed(LastCounter, EndCounter);
            LastCounter = EndCounter;
          }
          
          {
            FILETIME NewDllWriteTime = Win32GetLastWriteTime(GameCodeDllName);
            if(CompareFileTime(&Game.LastWriteTime, &NewDllWriteTime))
            {
              Win32UnloadGameCode(&Game);
              Game = Win32LoadGameCode(GameCodeDllName, GameCodeTempDllName, GameCodeLockName);
            }
          }
          
          GlobalState->GameInput.Keyboard.Backspace = false;
          {
            MSG Message;
            while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
            {
              TranslateMessage(&Message);
              DispatchMessageA(&Message);
            }
          }
          
          {
            game_controller_input *Input = &GlobalState->GameInput.Controller;
            
            XINPUT_STATE ControllerState = {0};
            XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
            if(XInputGetState(0, &ControllerState) == ERROR_SUCCESS)
            {
              Input->Connected = true;
              
              Input->Up = Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
              Input->Down = Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
              Input->Left= Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
              Input->Right= Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
              
              Input->Start = Pad->wButtons & XINPUT_GAMEPAD_START;
              
              Input->LeftShoulder= Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
              Input->RightShoulder = Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
              
              Input->A = Pad->wButtons & XINPUT_GAMEPAD_A;
              Input->B = Pad->wButtons & XINPUT_GAMEPAD_B;
              Input->X = Pad->wButtons & XINPUT_GAMEPAD_X;
              Input->Y = Pad->wButtons & XINPUT_GAMEPAD_Y;
              
              Input->LeftTrigger = Pad->bLeftTrigger;
              Input->RightTrigger = Pad->bLeftTrigger;
              
              Input->LeftThumbY = (Pad->sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? Pad->sThumbLX : 0;
              Input->LeftThumbX = (Pad->sThumbLY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? Pad->sThumbLY : 0;
              
              XINPUT_VIBRATION Vibration;
              Vibration.wLeftMotorSpeed = Input->LeftVibration;
              Vibration.wRightMotorSpeed = Input->RightVibration;
              XInputSetState(0, &Vibration);
            }
            else
            {
              Input->Connected = false;
              // NOTE(evan): Controller not connected, this is NOT an error
            }
          }
          
          {
            window_dimension NewWindowDimension = Win32GetWindowDimension(GlobalState->Window);
            if(GlobalState->D3D11State.RTView == 0 ||
               NewWindowDimension.Width != GlobalState->WindowDimension.Width ||
               NewWindowDimension.Height != GlobalState->WindowDimension.Height)
            {
              D3D11Resize(&GlobalState->D3D11State, NewWindowDimension,
                          &GlobalState->Platform);
              
              GlobalState->WindowDimension = NewWindowDimension;
            }
          }
          
          D3D11StartFrame(&GlobalState->D3D11State);
          
          ShouldClose = Game.GameUpdateAndRender(&GameMemory, &GlobalState->GameInput,
                                                 GlobalState->WindowDimension, DeltaTime);
          ShouldClose |= GlobalState->WindowClosed;
          
          D3D11EndFrame(&GlobalState->D3D11State, &GlobalState->Platform);
          
          GlobalState->TempArena.Used = 0;
        }
      }
    }
    else
    {
      Win32LogMessagePlain("Failed to create window\n", false, MESSAGE_SEVERITY_ERROR);
      return(1);
    }
  }
  else
  {
    Win32LogMessagePlain("Failed to register window class\n", false, MESSAGE_SEVERITY_ERROR);
    return(1);
  }
  
  return(0);
}

void __stdcall
WinMainCRTStartup(void)
{
  int Result = WinMain(GetModuleHandle(0), 0, 0, 0);
  ExitProcess(Result);
}

internal LRESULT CALLBACK
Win32WindowProc(HWND Window,
                UINT Message,
                WPARAM WParam,
                LPARAM LParam)
{
  LRESULT Result = 0;
  
  switch(Message)
  {
    case WM_SETCURSOR:
    {
      if(GlobalState->ShowCursor)
      {
        Result = DefWindowProc(Window, Message, WParam, LParam);
      }
      else
      {
        ShowCursor(0);
      }
    } break;
    
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
      u32 VKCode = (u32)WParam;
      b32 WasDown = LParam & (1 << 30);
      b32 IsDown = (LParam & (1 << 31)) == 0;
      
      if(WasDown != IsDown)
      {
        switch(VKCode)
        {
          case 'W':
          {
            GlobalState->GameInput.Keyboard.W = IsDown;
          } break;
          
          case 'A':
          {
            GlobalState->GameInput.Keyboard.A = IsDown;
          } break;
          
          case 'S':
          {
            GlobalState->GameInput.Keyboard.S = IsDown;
          } break;
          
          case 'D':
          {
            GlobalState->GameInput.Keyboard.D = IsDown;
          } break;
          
          case 'Q':
          {
            GlobalState->GameInput.Keyboard.Q = IsDown;
          } break;
          
          case 'E':
          {
            GlobalState->GameInput.Keyboard.E = IsDown;
          } break;
          
          case 'M':
          {
            GlobalState->GameInput.Keyboard.M = IsDown;
          } break;
          
          case VK_UP:
          {
            GlobalState->GameInput.Keyboard.Up = IsDown;
          } break;
          
          case VK_LEFT:
          {
            GlobalState->GameInput.Keyboard.Left = IsDown;
          } break;
          
          case VK_DOWN:
          {
            GlobalState->GameInput.Keyboard.Down = IsDown;
          } break;
          
          case VK_RIGHT:
          {
            GlobalState->GameInput.Keyboard.Right = IsDown;
          } break;
          
          case VK_BACK:
          {
            if(IsDown)
            {
              GlobalState->GameInput.Keyboard.Backspace = true;
            }
          } break;
          
          case VK_ESCAPE:
          {
            GlobalState->GameInput.Keyboard.Escape = IsDown;
          } break;
          
          case VK_DELETE:
          {
            GlobalState->GameInput.Keyboard.Delete = IsDown;
          } break;
          
          case VK_SPACE:
          {
            GlobalState->GameInput.Keyboard.Space = IsDown;
          } break;
          
          case VK_RETURN:
          {
            GlobalState->GameInput.Keyboard.Enter = IsDown;
          } break;
          
          case 0x31:
          {
            GlobalState->GameInput.Keyboard.One = IsDown;
          } break;
          
          case 0x32:
          {
            GlobalState->GameInput.Keyboard.Two = IsDown;
          } break;
          
          case 0x33:
          {
            GlobalState->GameInput.Keyboard.Three = IsDown;
          } break;
        }
      }
      
      if(IsDown)
      {
        b32 AltKeyIsDown = (LParam & (1 << 29));
        GlobalState->GameInput.Keyboard.AltF4 = false;
        if(AltKeyIsDown && VKCode == VK_F4)
        {
          GlobalState->GameInput.Keyboard.AltF4 = true;
        }
      }
    } break;
    
    case WM_LBUTTONDOWN:
    {
      GlobalState->GameInput.Mouse.LButton = true;
    } break;
    
    case WM_LBUTTONUP:
    {
      GlobalState->GameInput.Mouse.LButton = false;
    } break;
    
    case WM_RBUTTONDOWN:
    {
      GlobalState->GameInput.Mouse.RButton = true;
    } break;
    
    case WM_RBUTTONUP:
    {
      GlobalState->GameInput.Mouse.RButton = false;
    } break;
    
    case WM_MOUSEMOVE:
    {
      GlobalState->GameInput.Mouse.X = GET_X_LPARAM(LParam);
      GlobalState->GameInput.Mouse.Y = GET_Y_LPARAM(LParam);
    } break;
    
    case WM_CLOSE:
    {
      GlobalState->WindowClosed = true;
    } break;
    
    case WM_DESTROY:
    {
      GlobalState->WindowClosed = true;
    } break;
    
    default:
    {
      Result = DefWindowProc(Window, Message, WParam, LParam);
    } break;
  }
  
  return(Result);
}
