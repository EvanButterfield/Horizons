/* date = February 26th 2024 0:04 pm */

#ifndef WORLD_H
#define WORLD_H

#define MAX_CHUNKS 64

typedef struct world_chunk
{
  game_sprites SpriteIndex;
  f32 Y;
} world_chunk;

internal s32
LoadWorld(s8 *FileName, world_chunk **WorldChunks, platform_api *Platform,
          memory_arena *TempArena, memory_arena *PermArena)
{
  string8 FileNameStr = CreateString(FileName, TempArena, Platform);
  platform_file_handle File = Platform->OpenFile(FileNameStr, FILE_OPEN_READ);
  u32 FileSize = Platform->GetFileSize(File);
  s8 *Data = (s8 *)PushSize(TempArena, FileSize);
  Platform->ReadEntireFile(File, FileSize, Data);
  Platform->CloseFile(File);
  
  s32 NumNumbers = 0;
  s32 NumLengths[MAX_CHUNKS*2];
  s32 CurrentLength = 0;
  b32 LastWasNum = false;
  s8 *StartChar;
  for(s8 *Char = Data; Char < Data + FileSize; ++Char)
  {
    if(*Char == ' ' || *Char == '\r' || *Char == '\n' ||
       ((Char == Data + FileSize - 1) && LastWasNum))
    {
      if(LastWasNum)
      {
        NumLengths[NumNumbers++] = CurrentLength;
        CurrentLength = 0;
        LastWasNum = false;
        
        if(NumNumbers >= MAX_CHUNKS*2)
        {
          Platform->LogMessagePlain("Too many world chunks in file, using the max\n",
                                    true, MESSAGE_SEVERITY_WARNING);
          break;
        }
      }
      
      continue;
    }
    
    if((*Char < '0' || *Char > '9') && *Char != '-')
    {
      Platform->LogMessagePlain("Invalid character in world file: ", true,
                                MESSAGE_SEVERITY_ERROR);
      Platform->LogMessagePlain(FileName, true, MESSAGE_SEVERITY_ERROR);
      Platform->LogMessagePlain("\n\n", true, MESSAGE_SEVERITY_ERROR);
      return(0);
    }
    
    if(NumNumbers == 0 && CurrentLength == 0)
    {
      StartChar = Char;
    }
    
    ++CurrentLength;
    LastWasNum = true;
  }
  
  if(NumNumbers % 2 != 0)
  {
    Platform->LogMessagePlain("Incorrect format for world file: ", true,
                              MESSAGE_SEVERITY_ERROR);
    Platform->LogMessagePlain(FileName, true, MESSAGE_SEVERITY_ERROR);
    Platform->LogMessagePlain("\n\n", true, MESSAGE_SEVERITY_ERROR);
    return(0);
  }
  
  string8 *NumStrings = PushArray(TempArena, string8, NumNumbers);
  
  NumStrings[0] = CreateStringWithLength(StartChar, NumLengths[0],
                                         TempArena, Platform);
  s32 StringIndex = 1;
  LastWasNum = false;
  for(s8 *Char = StartChar + NumLengths[0]; Char < Data + FileSize; ++Char)
  {
    if(*Char == ' ' || *Char == '\r' || *Char == '\n')
    {
      LastWasNum = false;
      continue;
    }
    
    if(LastWasNum == false)
    {
      NumStrings[StringIndex] = CreateStringWithLength(Char, NumLengths[StringIndex],
                                                       TempArena, Platform);
      Char += NumLengths[StringIndex++];
      continue;
    }
    
    LastWasNum = true;
  }
  
  s32 NumChunks = NumNumbers/2;
  world_chunk *Chunks = PushArray(PermArena, world_chunk, NumChunks);
  s32 ChunkIndex = 0;
  for(s32 ChunkStringIndex = 0; ChunkStringIndex < NumNumbers; ChunkStringIndex += 2)
  {
    world_chunk *Chunk = &Chunks[ChunkIndex];
    string8 SpriteIndexString = NumStrings[ChunkStringIndex];
    string8 YString = NumStrings[ChunkStringIndex + 1];
    
    s32 OrderOfMagnitude = 1;
    for(s32 CharIndex = SpriteIndexString.Length - 1; CharIndex >= 0; --CharIndex)
    {
      s8 Digit = SpriteIndexString.Str[CharIndex];
      if(CharIndex == 0 && Digit == '-')
      {
        Chunk->SpriteIndex *= -1;
        break;
      }
      
      s32 Digit32 = Digit - '0'; // Offset the ascii code
      Chunk->SpriteIndex += Digit32*OrderOfMagnitude;
      OrderOfMagnitude *= 10;
    }
    
    OrderOfMagnitude = 1;
    for(s32 CharIndex = YString.Length - 1; CharIndex >= 0; --CharIndex)
    {
      s8 Digit = YString.Str[CharIndex];
      if(CharIndex == 0 && Digit == '-')
      {
        Chunk->SpriteIndex *= -1;
        break;
      }
      
      s32 Digit32 = Digit - '0'; // Offset the ascii code
      Chunk->Y += Digit32*OrderOfMagnitude;
      OrderOfMagnitude *= 10;
    }
    
    ++ChunkIndex;
  }
  
  *WorldChunks = Chunks;
  return(NumChunks);
}

#endif //WORLD_H
