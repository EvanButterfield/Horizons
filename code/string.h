/* date = February 4th 2024 4:34 pm */

#ifndef STRING_H
#define STRING_H

// TODO(evan): printf style logging

internal s32
StringLength(s8 *Str)
{
  s32 Count = 0;
  while(Str[Count] != 0)
  {
    ++Count;
  }
  
  return(Count);
}

internal string8
CreateString(s8 *Str, memory_arena *Arena, platform_api *Platform)
{
  string8 Result;
  Result.Length = StringLength(Str);
  Result.Str = PushArray(Arena, s8, Result.Length);
  Platform->CopyMemory(Result.Str, Str, Result.Length*sizeof(s8));
  
  return(Result);
}

internal string8
CreateStringWithLength(s8 *Str, s32 Length, memory_arena *Arena,
                       platform_api *Platform)
{
  string8 Result;
  Result.Length = Length;
  Result.Str = PushArray(Arena, s8, Result.Length);
  Platform->CopyMemory(Result.Str, Str, Result.Length*sizeof(s8));
  
  return(Result);
}

internal string8
DuplicateString(string8 String, memory_arena *Arena, platform_api *Platform)
{
  string8 Result;
  Result.Length = String.Length;
  Result.Str = PushArray(Arena, s8, Result.Length);
  Platform->CopyMemory(Result.Str, String.Str, Result.Length*sizeof(s8));
  
  return(Result);
}

internal string8
CatStrings(string8 First, string8 Second, memory_arena *Arena, platform_api *Platform)
{
  string8 Result;
  Result.Length = First.Length + Second.Length;
  Result.Str = PushArray(Arena, s8, Result.Length);
  Platform->CopyMemory(Result.Str, First.Str, First.Length);
  Platform->CopyMemory(Result.Str + First.Length, Second.Str, Second.Length);
  
  return(Result);
}

internal string8
CatStringsPlain(string8 First, s8 *Second, memory_arena *Arena,
                platform_api *Platform)
{
  s32 Length = StringLength(Second);
  string8 TempSecond = {Second, Length};
  
  string8 Result = CatStrings(First, TempSecond, Arena, Platform);
  return(Result);
}

#endif //STRING_H
