#pragma once

#include <stdint.h>

#if __ORBIS__
using wchar_t_t = wchar_t;
#elif __OPENORBIS__
using wchar_t_t = uint16_t;
#else
using wchar_t_t = wchar_t;
#endif
