/* date = February 4th 2024 4:34 pm */

#ifndef STRING_H
#define STRING_H

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
CreateString(s8 *Str, memory_arena *Arena, platform_copy_memory *CopyMemory)
{
  string8 Result;
  Result.Length = StringLength(Str);
  Result.Str = PushArray(Arena, s8, Result.Length);
  CopyMemory(Result.Str, Str, Result.Length*sizeof(s8));
  
  return(Result);
}

internal string8
DuplicateString(string8 String, memory_arena *Arena, platform_copy_memory *CopyMemory)
{
  string8 Result;
  Result.Length = String.Length;
  Result.Str = PushArray(Arena, s8, Result.Length);
  CopyMemory(Result.Str, String.Str, Result.Length*sizeof(s8));
  
  return(Result);
}

internal string8
IntToString(s32 Int, memory_arena *Arena, platform_copy_memory *CopyMemory)
{
  // Credit to: John Boker
  // https://stackoverflow.com/questions/3982320/convert-integer-to-string-without-access-to-libraries
  
  s8 Buffer[12];
  
  if(Int == 0)
  {
    Buffer[0] = '0';
    Buffer[1] = '\0';
  }
  else
  {
    b32 Neg = Int<0;
    
    u32 Unsigned = Neg ? -Int : Int;
    s32 Index = 0;
    while(Unsigned != 0)
    {
      Buffer[Index++] = Unsigned % 10 + '0';
      Unsigned = Unsigned/10;
    }
    
    if(Neg)
    {
      Buffer[Index++] = '-';
    }
    
    Buffer[Index] = '\0';
    
    for(s32 T = 0; T < Index/2; ++T)
    {
      Buffer[T] ^= Buffer[Index - T - 1];
      Buffer[Index - T - 1] ^= Buffer[T];
      Buffer[T] ^= Buffer[Index - T - 1];
    }
  }
  
  string8 Result = CreateString(Buffer, Arena, CopyMemory);
  return(Result);
}

internal string8
CatStrings(string8 First, string8 Second, memory_arena *Arena, platform_copy_memory *CopyMemory)
{
  string8 Result;
  Result.Length = First.Length + Second.Length;
  Result.Str = PushArray(Arena, s8, Result.Length);
  CopyMemory(Result.Str, First.Str, First.Length);
  CopyMemory(Result.Str + First.Length, Second.Str, Second.Length);
  
  return(Result);
}

internal string8
CatStringsPlain(string8 First, s8 *Second, memory_arena *Arena, platform_copy_memory *CopyMemory)
{
  s32 Length = StringLength(Second);
  string8 TempSecond = {Second, Length};
  
  string8 Result = CatStrings(First, TempSecond, Arena, CopyMemory);
  return(Result);
}

#endif //STRING_H
