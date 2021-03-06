#ifndef PLATFORM_H
#define PLATFORM_H

// [BB] Some Winelib specfic things.
#ifdef __WINE__
#ifdef _WIN32
#ifdef unix
#undef unix
#endif
#endif

#include <cmath>
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define fabsf(x) static_cast<float>(fabs(x))
#define ceilf(x) static_cast<float>(ceil(x))
#define atan2f(x,y) static_cast<float>(atan2(x,y))
#define fmodf(x,y) static_cast<float>(fmod(x,y))
#define modff(x,y)  ((float)modf((double)(x), (double *)(y)))
#endif

// [BB] Linux specific thigs, mostly missing functions.
#ifndef _WIN32

#define stricmp(x,y) strcasecmp(x,y)
#define _stricmp(x,y) strcasecmp(x,y)

typedef unsigned char UCHAR;
typedef bool BOOL;

// [C]
typedef unsigned short USHORT;
typedef short SHORT;

#endif

// [BB] FreeBSD specific defines
#ifdef __FreeBSD__
#define __va_copy(x,y) va_copy(x,y)
#endif

#ifdef _MSC_VER
// [BB] Silence the "'stricmp': The POSIX name for this item is deprecated." warning.
#pragma warning(disable:4996)
#endif

#endif
