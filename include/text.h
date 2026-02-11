#pragma once

#include <cctype>
#include <memory>
#include <string>

enum class TextEncoding {
  Utf8,
  Windows1252,
};

class StringSlice {
private:
  std::shared_ptr<std::string> _source;

  size_t _l;

  size_t _r;
  
public:
  StringSlice(std::string input)
    : _source(std::make_shared<std::string>(input))
    , _l(0)
    , _r(input.length())
  {}

  StringSlice(const StringSlice & other)
    : _source(other._source)
    , _l(other._l)
    , _r(other._r)
  {}

  size_t get_l() const {
    return _l;
  }

  size_t get_r() const {
    return _r;
  }

  size_t get_length() const {
    if (!check_bounds()) {
      return 0;
    }

    return _r - _l;
  }

  bool check_bounds() const {
    if (_l > _r) {
      return false;
    }
    
    const auto len = _source.get()->length();
    
    if (_l >= len) {
      return false;   
    }

    if (_r >= len) {
      return false;
    }

    return true;
  }

  const char * c_str() const {
    if (!check_bounds()) {
      return nullptr;
    }

    return &_source.get()->c_str()[_l];
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
      left.get_length(),
      right.get_length(),
      ignore_case);
  }
};
