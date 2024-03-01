/* date = December 8th 2023 9:06 am */

#ifndef TYPES_H
#define TYPES_H

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
// TODO(evan): More compilers!
#undef COMPILER_LLVM
#define COMPILER_LLVM
#endif
#endif

#if _WIN32|_WIN64
#define OS_WIN 1
#endif

#if unix|__unix|__unix__
#define OS_UNIX 1
#endif

#if __APPLE__
#define OS_MAC 1
#endif

#define internal static
#define global static

#define true 1
#define false 0

#define Kibibytes(Value) (Value*1024)
#define Mebibytes(Value) (Kibibytes(Value)*1024LL)
#define Gibibytes(Value) (Mebibytes(Value)*1024LL)
#define Tebibytes(Value) (Gibibytes(Value)*1024LL)

#define Pi32 3.14159265359f
#define DEG_TO_RAD (Pi32 / 180)
#define EPSILON 1.19209290E-07F

#if HORIZONS_DEBUG
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define STR2(X) #X
#define STR(X) STR2(X)

#define MAX_ENTITIES 128

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t  s64;

typedef float f32;
typedef double f64;

typedef s32 b32;

typedef size_t memory_index;

typedef struct string8
{
  s8 *Str;
  s32 Length;
} string8;

internal inline u32
SafeTruncateUInt64(u64 Value)
{
  Assert(Value <= 0xFFFFFFFF);
  u32 Result = (u32)Value;
  return(Result);
}

#endif //TYPES_H
