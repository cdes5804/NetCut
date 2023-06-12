#ifndef COLOR_HH
#define COLOR_HH

#include <iostream>

namespace color {
enum class Code {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_BLUE = 34,
  FG_DEFAULT = 39,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_BLUE = 44,
  BG_DEFAULT = 49
};

class Modifier {
 private:
  Code code_;

 public:
  explicit Modifier(Code pCode) : code_(pCode) {}
  friend std::ostream &operator<<(std::ostream &out_stream, const Modifier &mod) {
    return out_stream << "\033[" << static_cast<int32_t>(mod.code_) << "m";
  }
};
}  // namespace color

#endif