#pragma once
#include <stdint.h>

// sony SDK/proper types
#if defined(__ORBIS__)
typedef wchar_t wchar_t_t;
// OpenOrbis, wchar_t is marked as a 32-bit type(pre-fix) so we need to manually swap it out for a 16-bit type
#elif defined(__OPENORBIS__)
typedef uint16_t wchar_t_t;
#elif defined(__SWITCH__)
typedef wchar_t wchar_t_t;
#endif
