#pragma once

#include <string>
#include <vector>

#include "type.h"
#include "result.h"

namespace kinetic {

class Utf8 {
public:
  static Result<std::u32string> from(const std::string & input) {
    using ResultT = Result<std::u32string>;

    std::vector<c32> buffer;

    for (size_t r = 0; r < input.size(); ) {
      c32 v = 0;

      u8 b = static_cast<u8>(input[r]);
      u8 m = 0;

      if ((b & 0x80) == 0) {
        v = b;
      } else if ((b & 0xE0) == 0xC0) {
        v = b & 0x1F;
        m = 1;
      } else if ((b & 0xF0) == 0xE0) {
        v = b & 0x0F;
        m = 2;
      } else if ((b & 0xF8) == 0xF0) {
        v = b & 0x07;
        m = 3;
      } else {
        return ResultT::err(Error(ErrorKind::ValueInvalid, "invalid utf-8"));
      }

      r++;

      for (u8 i = 0; i < m; ++i) {
          if (r >= input.size()) {
            return ResultT::err(Error(ErrorKind::ValueEnded, "truncated utf-8"));
          }

          u8 next = static_cast<u8>(input[r]);
          if ((next & 0xC0) != 0x80) {
            return ResultT::err(Error(ErrorKind::ValueInvalid, "invalid continuation byte"));
          }

          v = (v << 6) | (next & 0x3F);
          r++;
      }

      buffer.emplace_back(v);
    }

    std::u32string str(buffer.begin(), buffer.end());

    return ResultT::ok(str);
  }

  static Result<std::string> to_string(const std::u32string & input) {
    using ResultT = Result<std::string>;

    std::string str;

    str.reserve(input.size());

    for (const char32_t v : input) {
      if (v <= 0x7F) {
        str += static_cast<char>(v);
      } else if (v <= 0x7FF) {
        str += static_cast<char>(0xC0 | ((v >>  6) & 0x1F));
        str += static_cast<char>(0x80 | ( v        & 0x3F));
      } else if (v <= 0xFFFF) {
        str += static_cast<char>(0xE0 | ((v >> 12) & 0x0F));
        str += static_cast<char>(0x80 | ((v >>  6) & 0x3F));
        str += static_cast<char>(0x80 | ( v        & 0x3F));
      } else if (v <= 0x10FFFF) {
        str += static_cast<char>(0xF0 | ((v >> 18) & 0x07));
        str += static_cast<char>(0x80 | ((v >> 12) & 0x3F));
        str += static_cast<char>(0x80 | ((v >>  6) & 0x3F));
        str += static_cast<char>(0x80 | ( v        & 0x3F));
      } else {
        throw std::invalid_argument("invalid codepoint in u32string");
      }
    }

    return ResultT::ok(str);
  }
};

}
