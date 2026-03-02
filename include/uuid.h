#pragma once

#include <cstdio>
#include <cstdint>
#include <array>
#include <ostream>
#include <random>

#include "type.h"
#include "meta.h"

namespace kinetic {

using UuidInner = std::array<u8, 16>;

enum class UuidVersion {
  None,
  V1,
  V2,
  V3,
  V4,
  V5,
  V6,
  V7,
  V8,
};

class Uuid {
private:
  const UuidVersion _version;

  const UuidInner _inner;

  static UuidInner into_array(const u64 u, const u64 l) {
    UuidInner result;

    // Upper 8 bytes (big-endian)
    result[0]  = static_cast<u8>(u >> 56);
    result[1]  = static_cast<u8>(u >> 48);
    result[2]  = static_cast<u8>(u >> 40);
    result[3]  = static_cast<u8>(u >> 32);
    result[4]  = static_cast<u8>(u >> 24);
    result[5]  = static_cast<u8>(u >> 16);
    result[6]  = static_cast<u8>(u >>  8);
    result[7]  = static_cast<u8>(u >>  0);

    // Lower 8 bytes (big-endian)
    result[8]  = static_cast<u8>(l >> 56);
    result[9]  = static_cast<u8>(l >> 48);
    result[10] = static_cast<u8>(l >> 40);
    result[11] = static_cast<u8>(l >> 32);
    result[12] = static_cast<u8>(l >> 24);
    result[13] = static_cast<u8>(l >> 16);
    result[14] = static_cast<u8>(l >>  8);
    result[15] = static_cast<u8>(l >>  0);

    return result;
  }

  Uuid(const UuidVersion version, const u64 upper, const u64 lower)
    : _version(version)
    , _inner(into_array(upper, lower))
  {}

public:
  static Uuid new_nil() {
    return Uuid(UuidVersion::None, 0, 0);
  }

  static Uuid new_v4() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<u64> dist(0, UINT64_MAX);

    u64 upper = dist(gen);
    u64 lower = dist(gen);

    // Set version 4: bits 12-15 of upper (time_hi_and_version)
    upper = (upper & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;

    // Set variant 1 (10xx): bits 62-63 of lower (clock_seq_hi)
    lower = (lower & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    return Uuid(UuidVersion::V4, upper, lower);
  }

  KINETIC_GETTER(_version, version)

  bool operator==(const Uuid & other) const {
    return other._inner == _inner;
  }

  bool operator!=(const Uuid & other) const {
    return other._inner != _inner;
  }

  std::string to_string() const {
    char buf[37] = {0};

    std::snprintf(buf, sizeof(buf),
      "%02x%02x%02x%02x-"
      "%02x%02x-"
      "%02x%02x-"
      "%02x%02x-"
      "%02x%02x%02x%02x%02x%02x",
      _inner[0x0], _inner[0x1], _inner[0x2], _inner[0x3],
      _inner[0x4], _inner[0x5],
      _inner[0x6], _inner[0x7],
      _inner[0x8], _inner[0x9],
      _inner[0xA], _inner[0xB], _inner[0xC], _inner[0xD], _inner[0xE], _inner[0xF]);

    const std::string result(buf);

    return result;
  }
};

inline std::ostream & operator<<(std::ostream & out, const Uuid & uuid) {
  out << uuid.to_string();

  return out;
}

}
