#pragma once

#include "../../_prelude.hpp"
#include "../../io/Stream.hpp"
#include "../../io/Socket.hpp"

namespace KINETIC_NAMESPACE {

class TcpStream : public Stream {
private:
  std::shared_ptr<Socket> _socket;

public:
  TcpStream(const std::shared_ptr<Socket> & socket)
    : _socket(socket)
  {}

  TcpStream(const int fd)
    : _socket(std::make_shared<Socket>(fd))
  {}

  ~TcpStream() override {
    if (_socket == nullptr) {
      return;
    }

    _socket->close();
  };

  Result<Unit> close() {
    if (_socket == nullptr) {
      return {{}};
    }

    return _socket->close();
  }

  Result<usize> read(u8 * into, const usize limit) override {
    if (_socket == nullptr) {
      return {ErrorKind::ValueNone, "socket is null"};
    }

    return _socket->read(into, limit);
  }

  Result<usize> write(const u8 * from, const usize limit) override {
    if (_socket == nullptr) {
      return {ErrorKind::ValueNone, "socket is null"};
    }

    return _socket->write(from, limit);
  }

  static Result<std::shared_ptr<KINETIC_NAMESPACE::TcpStream>> dial(
    const std::string & addr,
    const u16           port
  ) {
    const auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      return {ErrorKind::PlatformError, "socket() fail"};
    }

    int opt = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    if (inet_pton(AF_INET, addr.c_str(), &a.sin_addr) != 1) {
      ::close(fd);
      return {ErrorKind::ValueIncorrect, "inet_pton() fail"};
    }

    if (::connect(fd, (sockaddr *) &a, sizeof(a)) < 0) {
      ::close(fd);
      return {ErrorKind::PlatformError, "connect() fail"};
    }

    return {std::make_shared<TcpStream>(fd)};
  }
};

}
