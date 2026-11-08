#pragma once

#include "../_prelude.hpp"
#include "./Stream.hpp"

#if   defined(PLATFORM_FLAG_LINUX)
#define KINETIC_IO_SOCKET_NOSIGPIPE MSG_NOSIGNAL
#else
#error "unimplemented"
#endif

namespace KINETIC_NAMESPACE {

class Socket : public Stream {
private:
  int _fd;

public:
  Socket(const int fd)
    : _fd(fd)
  {}

  ~Socket() override {
    close();
  }

  Socket(const Socket&) = delete;

  Socket & operator=(const Socket &) = delete;

  Socket(Socket &&) noexcept = delete;

  Socket & operator=(Socket &&) noexcept = delete;

  int get_fd() const {
    return _fd;
  }

  Result<Unit> close() {
    if (_fd < 0) {
      return {{}};
    }

    if (::shutdown(_fd, SHUT_WR) != 0) {
      return {ErrorKind::PlatformError, "shutdown() fail"};
    }

    const auto op_r = ::close(_fd);
    _fd = -1;
    if (op_r != 0) {
      return {ErrorKind::PlatformError, "close() fail"};
    }

    return {{}};
  }

  Result<usize> read(u8 * into, const usize limit) override {
    if (into == nullptr) {
      return {ErrorKind::ValueNone, "into is null"};
    }

    if (limit == 0) {
      return {0};
    }

    if (_fd < 0) {
      return {ErrorKind::ValueUnavailable, "invalid socket fd"};
    }

    ssize io_r = {};
    do {
      io_r = ::recv(_fd, into, limit, 0);
    } while (io_r < 0 && errno == EINTR);
    if (io_r < 0) {
      return {ErrorKind::PlatformError, "recv() fail"};
    }

    return {static_cast<usize>(io_r)};
  }

  Result<usize> write(const u8 * from, const usize limit) override {
    if (from == nullptr) {
      return {ErrorKind::ValueNone, "from is null"};
    }

    if (limit == 0) {
      return {0};
    }

    if (_fd < 0) {
      return {ErrorKind::ValueUnavailable, "invalid socket fd"};
    }

    ssize io_r = {};
    do {
        io_r = ::send(_fd, from, limit, KINETIC_IO_SOCKET_NOSIGPIPE);
    } while (io_r < 0 && errno == EINTR);
    if (io_r < 0) {
      return {ErrorKind::PlatformError, "send() fail"};
    }

    return {static_cast<usize>(io_r)};
  }
};

}
