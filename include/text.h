#pragma once

#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>

#include "slice.h"

namespace kinetic {

enum class TextEncoding {
  Utf8,
  Windows1252,
};

class StringSlice : public kinetic::Slice<char> {
private:
  static std::vector<char> vec_from_str(const std::string & input) {
    std::vector<char> result; // Todo: avoid copy?

    for (const char it : input) {
      result.push_back(it);
    }

    return result;
  }

public:
  StringSlice(std::string input)
    : kinetic::Slice<char>(std::make_shared<std::vector<char>>(vec_from_str(input)))
  {}

  StringSlice(const kinetic::Slice<char> &slice)
    : kinetic::Slice<char>(slice)
  {}

  const char * c_str() const {
    if (get_start() >= get_source_len()) {
      return nullptr;
    }

    return &get_source()->data()[sizeof(char) * this->get_start()];
  }

  std::string to_string() const {
    if (!check()) {
      throw std::logic_error("invalid StringSlice bounds");
    }

    std::string result(c_str(), get_len());

    return result;
  }
};

class Text {
  static char to_lower(const char input) {
    return std::tolower(input);
  }

  static char to_upper(const char input) {
    return std::toupper(input);
  }

  static bool eq(
    const char   * left,
    const char   * right,
    const size_t   len_left,
    const size_t   len_right,
    const bool     ignore_case = false
  ) {
    if (left == nullptr) {
      return false;
    }

    if (right == nullptr) {
      return false;
    }

    if (len_left != len_right) {
      return false;
    }

    if (ignore_case) {
      for (size_t x = 0; x < len_left; x++) {
        if (Text::to_lower(left[x]) != Text::to_lower(right[x])) {
          return false;
        }
      }
    } else {
      for (size_t x = 0; x < len_left; x++) {
        if (left[x] != right[x]) {
          return false;
        }
      }
    }

    return true;
  }

  static bool eq(
    const StringSlice & left,
    const StringSlice & right,
    const bool          ignore_case = false
  ) {
    const char * c_left  = left.c_str();
    const char * c_right = right.c_str();

    if (c_left == nullptr) {
      return false;
    }

    if (c_right == nullptr) {
      return false;
    }

    return eq(
      c_left,
      c_right,
      left.get_len(),
      right.get_len(),
      ignore_case);
  }
};

}
