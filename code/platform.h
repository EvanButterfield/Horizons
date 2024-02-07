/* date = December 9th 2023 0:33 pm */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"

typedef struct game_keyboard_input
{
  b32 W;
  b32 A;
  b32 S;
  b32 D;
  b32 Q;
  b32 E;
  b32 M;
  
  b32 Up;
  b32 Left;
  b32 Down;
  b32 Right;
  
  b32 Backspace;
  b32 Escape;
  b32 Delete;
  b32 Space;
  b32 Enter;
  
  b32 One;
  b32 Two;
  b32 Three;
  
  b32 AltF4;
} game_keyboard_input;

typedef struct game_mouse_input
{
  b32 LButton;
  b32 RButton;
  
  s32 TargetX;
  s32 X;
  s32 TargetY;
  s32 Y;
} game_mouse_input;

typedef struct game_controller_input
{
  b32 Connected;
  b32 Up;
  b32 Left;
  b32 Down;
  b32 Right;
  b32 Start;
  b32 LeftShoulder;
  b32 RightShoulder;
  b32 A;
  b32 B;
  b32 X;
  b32 Y;
  s16 LeftTrigger;
  s16 RightTrigger;
  s16 LeftThumbX;
  s16 LeftThumbY;
  s16 LeftVibration;
  s16 RightVibration;
} game_controller_input;

typedef struct game_input
{
  game_keyboard_input Keyboard;
  game_mouse_input Mouse;
  game_controller_input Controller;
} game_input;

typedef struct window_dimension
{
  u32 Width;
  u32 Height;
} window_dimension;

typedef struct memory_arena
{
  u8 *Memory;
  memory_index Size;
  memory_index Used;
} memory_arena;

internal void
InitializeArena(memory_arena *Arena, u8 *BackingBuffer, memory_index Size)
{
  Arena->Memory = BackingBuffer;
  Arena->Size = Size;
  Arena->Used = 0;
}

#define PushStruct(Arena, type) (type *)PushSize(Arena, sizeof(type))
#define PushArray(Arena, type, Count) (type *)PushSize(Arena, sizeof(type)*Count)
#define PopArray(Arena, type, Count) (type *)PopSize(Arena, sizeof(type)*Count)
internal void *
PushSize(memory_arena *Arena, memory_index Size)
{
  void *Result = 0;
  if(Arena->Size >= Arena->Used + Size)
  {
    Result = Arena->Memory + Arena->Used;
    Arena->Used += Size;
  }
  
  return(Result);
}
internal void *
PopSize(memory_arena *Arena, memory_index Size)
{
  void *Result = 0;
  if(Arena->Used - Size >= 0)
  {
    Arena->Used -= Size;
    Result = Arena->Memory + Arena->Used;
  }
  
  return(Result);
}

typedef struct platform_file_handle
{
  void *Handle;
  string8 FileName;
} platform_file_handle;

typedef enum file_open_flags
{
  FILE_OPEN_NONE = 0,
  FILE_OPEN_READ = 0b1,
  FILE_OPEN_WRITE = 0b10
} file_open_flags;

#define PLATFORM_OPEN_FILE(name) platform_file_handle name(string8 FileName, file_open_flags Flags)
typedef PLATFORM_OPEN_FILE(platform_open_file);

#define PLATFORM_GET_FILE_SIZE(name) u32 name(platform_file_handle Handle)
typedef PLATFORM_GET_FILE_SIZE(platform_get_file_size);

#define PLATFORM_READ_ENTIRE_FILE(name) void name(platform_file_handle Handle, u32 FileSize, void *Dest)
typedef PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(platform_file_handle Handle, u32 DataSize, void *Data)
typedef PLATFORM_WRITE_ENTIRE_FILE(platform_write_entire_file);

#define PLATFORM_CLOSE_FILE(name) void name(platform_file_handle Handle)
typedef PLATFORM_CLOSE_FILE(platform_close_file);

typedef enum message_severity
{
  MESSAGE_SEVERITY_DEBUG,
  MESSAGE_SEVERITY_WARNING,
  MESSAGE_SEVERITY_ERROR,
  
  MESSAGE_SEVERITY_COUNT
} message_severity;
string8 SeverityMessages[MESSAGE_SEVERITY_COUNT] =
{ { "[DEBUG]:   ", 11},
  { "[WARNING]: ", 11},
  { "[ERROR]:   ", 11} };
#define PLATFORM_LOG_MESSAGE(name) void name(string8 Message, s32 FromEngine, message_severity Severity)
typedef PLATFORM_LOG_MESSAGE(platform_log_message);

#define PLATFORM_LOG_MESSAGE_PLAIN(name) void name(s8 *Message, s32 FromEngine, message_severity Severity)
typedef PLATFORM_LOG_MESSAGE_PLAIN(platform_log_message_plain);

#define PLATFORM_COPY_MEMORY(name) void name(void *Dest, void *Source, memory_index Length)
typedef PLATFORM_COPY_MEMORY(platform_copy_memory);

#define PLATFORM_ZERO_MEMORY(name) void name(void *Dest, memory_index Length)
typedef PLATFORM_ZERO_MEMORY(platform_zero_memory);

// TODO(evan): platform_api
typedef struct memory
{
  memory_index PermanentStorageSize;
  void *PermanentStorage;
  
  memory_index TempStorageSize;
  void *TempStorage;
  
  platform_open_file *OpenFile;
  platform_get_file_size *GetFileSize;
  platform_read_entire_file *ReadEntireFile;
  platform_write_entire_file *WriteEntireFile;
  platform_close_file *CloseFile;
  platform_log_message *LogMessage;
  platform_log_message_plain *LogMessagePlain;
  
  platform_copy_memory *CopyMemory;
  platform_zero_memory *ZeroMemory;
} memory;

#define GAME_UPDATE_AND_RENDER(name) b32 name(memory *Memory, game_input *Input, f32 DeltaTime)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
internal GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
  Memory->LogMessagePlain("Inside Stub\n", true, MESSAGE_SEVERITY_DEBUG);
  return(false);
}

#endif //PLATFORM_H
