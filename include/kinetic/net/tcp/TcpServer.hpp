#pragma once

#include "../../_prelude.hpp"
#include "../../io/Socket.hpp"
#include "kinetic/util/Error.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

namespace KINETIC_NAMESPACE {

class TcpServer {
private:
  std::string _addr;

  u16 _port;

  int _fd;

public:
  TcpServer(const std::string & addr, const u16 port)
    : _addr(addr)
    , _port(port)
    , _fd(-1)
  {}

  Result<Unit> open() {
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
      return {ErrorKind::PlatformError, "socket() fail"};
    }

    int opt = 1;
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ::setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _addr.c_str(), &addr.sin_addr) != 1) {
      close();
      return {ErrorKind::ValueIncorrect, "inet_pton() fail"};
    }

    if (::bind(_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0) {
      close();
      return {ErrorKind::PlatformError, "bind() fail"};
    }

    if (::listen(_fd, SOMAXCONN) < 0) {
      close();
      return {ErrorKind::PlatformError, "listen() fail"};
    }

    return {{}};
  }

  void close() {
    if (_fd < 0) {
      return;
    }

    ::close(_fd);
    _fd = -1;
  }

  Result<std::shared_ptr<Socket>> try_accept() {
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    const auto client_fd = ::accept(_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
    if (client_fd < 0) {
      if (errno == EINTR) {
        return {ErrorKind::ValueUnavailable, "accept() EINTR"};
      }
      if (errno == ECONNABORTED) {
        return {ErrorKind::ValueUnavailable, "accept() ECONNABORTED"};
      }

      return {ErrorKind::PlatformError, "accept() fail"};
    }

    // char client_ip_c[INET_ADDRSTRLEN] = {0};
    // inet_ntop(AF_INET, &client_addr.sin_addr, client_ip_c, INET_ADDRSTRLEN);
    // std::string client_ip = std::string(client_ip_c) + ":" + std::to_string(client_addr.sin_port);

    return {std::make_shared<Socket>(client_fd)};
  }

  Result<std::shared_ptr<Socket>> accept() {
    for (;;) {
      const auto r = try_accept();

      if (r.is_err()) {
        if (r.get_error().get_message() == "accept() EINTR") {
          continue; // EINTR
        }
        if (r.get_error().get_message() == "accept() ECONNABORTED") {
          continue; // ECONNABORTED
        }
      }

      return r;
    }
  }
};

}
