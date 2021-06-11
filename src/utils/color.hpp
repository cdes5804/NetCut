#ifndef _COLOR_HPP
#define _COLOR_HPP

#include <iostream>

namespace Color {
    enum class Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };

    class Modifier {
      private:
        Code code;
      public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << static_cast<int32_t>(mod.code) << "m";
        }
    };
}

#endif