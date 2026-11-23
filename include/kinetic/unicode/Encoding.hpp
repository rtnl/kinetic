#pragma once

#include "../_prelude.hpp"
#include "../util/Result.hpp"
#include "./UnicodeString.hpp"
#include "../io/Read.hpp"
#include "../io/Write.hpp"

namespace KINETIC_NAMESPACE {

enum class UnicodeEncoding {
  Utf8,
  Utf16LE,
  Utf16BE,
  Utf32LE,
  Utf32BE,
};

enum class UnicodeHandling {
  Strict,
  Replace,
  Ignore,
  Preserve,
  Escape,
};

template <UnicodeEncoding Encoding>
class UnicodeEncodingSpec {
public:
  template <UnicodeHandling Handling>
  static Result<usize> encode(const std::shared_ptr<Write> & writer);

  template <UnicodeHandling Handling>
  static Result<std::shared_ptr<UnicodeString>> decode(const std::shared_ptr<Read> & reader);
};

}
