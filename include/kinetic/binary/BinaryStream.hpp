#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

#include "../_prelude.hpp"
#include "../io/Stream.hpp"
#include "./ByteOrder.hpp"
#include "../util/Result.hpp"
#include "../util/Unit.hpp"

#define KINETIC_BINARYSTREAM_IMPL_W_INT_U(TYPE)                           \
  Result<Unit> KINETIC_CONCAT(write_, TYPE) (const TYPE value) noexcept { \
    return write_uint<TYPE>(value);                                       \
  }

#define KINETIC_BINARYSTREAM_IMPL_W_INT_S(TYPE, UTYPE)                    \
  Result<Unit> KINETIC_CONCAT(write_, TYPE) (const TYPE value) noexcept { \
    return write_uint<UTYPE>(static_cast<UTYPE>(value));                  \
  }

#define KINETIC_BINARYSTREAM_IMPL_R_INT_U(TYPE)           \
  Result<TYPE> KINETIC_CONCAT(read_, TYPE) () noexcept {  \
    return read_uint<TYPE>();                             \
  }

#define KINETIC_BINARYSTREAM_IMPL_R_INT_S(TYPE, UTYPE)   \
  Result<TYPE> KINETIC_CONCAT(read_, TYPE) () noexcept { \
    const auto value_r = KINETIC_CONCAT(read_, UTYPE)(); \
    if (value_r.is_err()) {                              \
      return {value_r.get_error()};                      \
    }                                                    \
    const UTYPE value = value_r.get();                   \
    return static_cast<TYPE>(value);                     \
  }

namespace KINETIC_NAMESPACE {

/// @brief: stream wrapper for binary types in big and little endian
template <ByteOrder O>
class BinaryStream {
  static_assert(
       O == ByteOrder::BigEndian
    || O == ByteOrder::LittleEndian,
    "BinaryStream only support BigEndian and LittleEndian");

private:
  std::shared_ptr<Stream> _stream;

public:
  Error ErrStreamNull{ErrorKind::ValueNone, "stream is null"};

  /// @brief BinaryStream constructor using Stream
  /// @param[in] stream non-null Stream shared pointer
  BinaryStream(const std::shared_ptr<Stream> & stream) noexcept
    : _stream(stream)
  {}

  ~BinaryStream() = default;

private:
  template <typename A>
  Result<Unit> write_uint(A value) noexcept;

  template <typename A>
  Result<A> read_uint() noexcept;

public:
  KINETIC_BINARYSTREAM_IMPL_W_INT_U(u8);
  KINETIC_BINARYSTREAM_IMPL_W_INT_U(u16);
  KINETIC_BINARYSTREAM_IMPL_W_INT_U(u32);
  KINETIC_BINARYSTREAM_IMPL_W_INT_U(u64);
  KINETIC_BINARYSTREAM_IMPL_W_INT_S(i8,   u8);
  KINETIC_BINARYSTREAM_IMPL_W_INT_S(i16,  u16);
  KINETIC_BINARYSTREAM_IMPL_W_INT_S(i32,  u32);
  KINETIC_BINARYSTREAM_IMPL_W_INT_S(i64,  u64);
  KINETIC_BINARYSTREAM_IMPL_W_INT_S(rune, u32);
  KINETIC_BINARYSTREAM_IMPL_R_INT_U(u8);
  KINETIC_BINARYSTREAM_IMPL_R_INT_U(u16);
  KINETIC_BINARYSTREAM_IMPL_R_INT_U(u32);
  KINETIC_BINARYSTREAM_IMPL_R_INT_U(u64);
  KINETIC_BINARYSTREAM_IMPL_R_INT_S(i8,   u8);
  KINETIC_BINARYSTREAM_IMPL_R_INT_S(i16,  u16);
  KINETIC_BINARYSTREAM_IMPL_R_INT_S(i32,  u32);
  KINETIC_BINARYSTREAM_IMPL_R_INT_S(i64,  u64);
  KINETIC_BINARYSTREAM_IMPL_R_INT_S(rune, u32);
};

template <>
template <typename A>
Result<Unit> BinaryStream<ByteOrder::BigEndian>::write_uint(A value) noexcept {
  static_assert(std::is_unsigned<A>::value, "write_uint() requires unsigned type");

  if (_stream == nullptr) {
    return ErrStreamNull;
  }

  std::array<u8, sizeof(A)> buf;

  for (usize x = 0; x < sizeof(A); x++) {
    buf[sizeof(A) - 1 - x] = static_cast<u8>(value & 0xFFu);

    value >>= 8;
  }

  return _stream->write_all(buf.data(), sizeof(A));
}

template <>
template <typename A>
Result<Unit> BinaryStream<ByteOrder::LittleEndian>::write_uint(A value) noexcept {
  static_assert(std::is_unsigned<A>::value, "write_uint() requires unsigned type");

  if (_stream == nullptr) {
    return ErrStreamNull;
  }

  std::array<u8, sizeof(A)> buf;

  for (usize x = 0; x < sizeof(A); x++) {
    buf[x] = static_cast<u8>(value & 0xFFu);
    value >>= 8;
  }

  return _stream->write_all(buf.data(), sizeof(A));
}

template <>
template <typename A>
Result<A> BinaryStream<ByteOrder::BigEndian>::read_uint() noexcept {
  static_assert(std::is_unsigned<A>::value, "read_uint() requires unsigned type");

  if (_stream == nullptr) {
    return ErrStreamNull;
  }

  std::array<u8, sizeof(A)> buf;

  const auto io_r = _stream->read_exact(buf.data(), sizeof(A));
  if (io_r.is_err()) {
    return {io_r.get_error()};
  }

  A value = 0;
  for (usize x = 0; x < sizeof(A); x++) {
    value = static_cast<A>((value << 8) | buf[x]);
  }

  return {value};
}

template <>
template <typename A>
Result<A> BinaryStream<ByteOrder::LittleEndian>::read_uint() noexcept {
  static_assert(std::is_unsigned<A>::value, "read_uint() requires unsigned type");

  if (_stream == nullptr) {
    return ErrStreamNull;
  }

  std::array<u8, sizeof(A)> buf;

  const auto io_r = _stream->read_exact(buf.data(), sizeof(A));
  if (io_r.is_err()) {
    return {io_r.get_error()};
  }

  A value = 0;
  for (usize x = 0; x < sizeof(A); x++) {
    value = static_cast<A>((value << 8) | buf[sizeof(A) - x - 1]);
  }

  return {value};
}

}

#pragma GCC diagnostic pop
