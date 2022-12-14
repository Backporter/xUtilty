#pragma once
#include <stdint.h>

#if defined(__ORBIS__)
typedef wchar_t wchar_t_t;
#elif defined(__OPENORBIS__)
typedef uint16_t wchar_t_t;
#elif defined(__SWITCH__)
typedef wchar_t wchar_t_t;
#endif
