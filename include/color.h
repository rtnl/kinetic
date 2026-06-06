#pragma once

#include <string>
#include <stdexcept>

namespace kinetic {

enum class ColorCode {
  Reset,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
};

static std::string ColorCode_str(const ColorCode code) {
  switch (code) {
    case ColorCode::Reset:   return "\033[0m";
    case ColorCode::Black:   return "\033[30m";
    case ColorCode::Red:     return "\033[31m";
    case ColorCode::Green:   return "\033[32m";
    case ColorCode::Yellow:  return "\033[33m";
    case ColorCode::Blue:    return "\033[34m";
    case ColorCode::Magenta: return "\033[35m";
    case ColorCode::Cyan:    return "\033[36m";
    case ColorCode::White:   return "\033[37m";
    default:
      throw std::runtime_error("unimplemented");
  }
}

static std::string ColorCode_wrap(const std::string & input, const ColorCode code) {
  return ColorCode_str(code) + input + ColorCode_str(ColorCode::Reset);
}

}
