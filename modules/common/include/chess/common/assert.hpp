
#pragma once

#include <cstdio>

#if CHESS_DEBUG
#define CHESS_BREAK_IN_DEBUGGER \
    { __builtin_debugtrap(); }

namespace chess {
    inline void dbg(const char* file, int line, const char* s) { printf("Debug(%s:%d): %s\n", file, line, s); }

    template<typename... Args> inline void dbg(const char* file, int line, const char* s, Args... args) {
        char string[4096];
        snprintf(string, 4096, s, args...);
        printf("Debug(%s:%d): %s\n", file, line, string);
    }
}

#define CHESS_DBG(...) chess::dbg(__FILE__, __LINE__, __VA_ARGS__);

#define CHESS_ASSERT(condition) \
    if ((!(condition))) { \
        CHESS_DBG("chess_assert failed") CHESS_BREAK_IN_DEBUGGER \
    } else { \
    }
#else
#define CHESS_BREAK_IN_DEBUGGER
#define CHESS_DBG(...)
#define CHESS_ASSERT(condition)
#endif
