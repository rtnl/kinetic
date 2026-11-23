#pragma once

#include "../_prelude.hpp"
#include "../util/Option.hpp"

namespace KINETIC_NAMESPACE {

class UnicodeString {
private:
  std::vector<rune> _data;

public:
  UnicodeString()
    : _data()
  {}

  ~UnicodeString() = default;

  usize get_length() const {
    return _data.size();
  }

  Option<rune> at(const usize index) const {
    if (index >= _data.size()) {
      return {};
    }

    return {_data[index]};
  }

  void push(const rune r) {
    _data.push_back(r);
  }

  void push(const UnicodeString & other) {
    _data.insert(_data.end(), other._data.begin(), other._data.end());
  }

  void push(const std::shared_ptr<UnicodeString> & other) {
    if (other == nullptr) {
      return;
    }

    push(*other);
  }
};

}
