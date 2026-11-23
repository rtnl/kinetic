#pragma once

#include "./Encoding.hpp"

static std::shared_ptr<kinetic::UnicodeString> operator""_utf8(const char * input, const usize len) {
  throw std::runtime_error("unimplemented"); // Todo: implement this
}

static std::shared_ptr<kinetic::UnicodeString> operator""_utf16le(const char * input, const usize len) {
  throw std::runtime_error("unimplemented"); // Todo: implement this
}

static std::shared_ptr<kinetic::UnicodeString> operator""_utf16be(const char * input, const usize len) {
  throw std::runtime_error("unimplemented"); // Todo: implement this
}

static std::shared_ptr<kinetic::UnicodeString> operator""_utf32le(const char * input, const usize len) {
  throw std::runtime_error("unimplemented"); // Todo: implement this
}

static std::shared_ptr<kinetic::UnicodeString> operator""_utf32be(const char * input, const usize len) {
  throw std::runtime_error("unimplemented"); // Todo: implement this
}
