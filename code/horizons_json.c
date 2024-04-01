#include "json.h"

internal struct json_value_s *
JSONFindValue(struct json_object_element_s *Element, string8 Name)
{
  while(!CompareStrings(String8(Element->name->string, (s32)Element->name->string_size), Name))
  {
    Element = Element->next;
  }
  
  struct json_value_s *Result = Element->value;
  return(Result);
}

internal f32
JSONGetFloatFromValue(struct json_value_s *Value)
{
  struct json_number_s *NumberS = json_value_as_number(Value);
  s8 *TempNumberString = PushArray(&State->TempArena, s8, NumberS->number_size + 1);
  Platform->CopyMemory(TempNumberString, NumberS->number, NumberS->number_size);
  TempNumberString[NumberS->number_size] = 0;
  f32 Number = (f32)atof(TempNumberString);
  PopArray(&State->TempArena, s8, NumberS->number_size + 1);
  
  return(Number);
}

internal s32
JSONGetS32FromValue(struct json_value_s *Value)
{
  struct json_number_s *NumberS = json_value_as_number(Value);
  s8 *TempNumberString = PushArray(&State->TempArena, s8, NumberS->number_size + 1);
  Platform->CopyMemory(TempNumberString, NumberS->number, NumberS->number_size);
  TempNumberString[NumberS->number_size] = 0;
  s32 Number = atoi(TempNumberString);
  PopArray(&State->TempArena, s8, NumberS->number_size + 1);
  
  return(Number);
}