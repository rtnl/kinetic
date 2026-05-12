#pragma once

#include <unit.h>
#include <reader.h>
#include <writer.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <memory>

namespace kinetic {

class TcpStream : public BufReader, public Writer {
private:
  int _socket_fd;

public:
  TcpStream(const int socket_fd)
    : _socket_fd(socket_fd)
  {}

  ~TcpStream() {
    if (_socket_fd >= 0) {
      ::close(_socket_fd);
    }
  }

  R_Size read(u8 * buf, usize size) override {
    ssize len = ::recv(_socket_fd, buf, size, 0);
    if (len == 0) {
      return R_Size::err(ErrorKind::ValueEnded, "stream eof");
    }
    if (len < 0) {
      return R_Size::err(ErrorKind::ValueInvalid, "recv failed");
    }

    return R_Size::ok(static_cast<usize>(len));
  }

  R_Size write(const u8 * buf, usize size) override {
    ssize len = ::send(_socket_fd, buf, size, 0);
    if (len < 0) {
      return R_Size::err(ErrorKind::ValueInvalid, "send failed");
    }

    return R_Size::ok(static_cast<usize>(len));
  }
};

class TcpServer {
private:
  std::string _host;

  u16 _port;

  int _server_fd;

public:
  TcpServer(const std::string & host, const u16 port)
    : _host(std::move(host))
    , _port(port)
    , _server_fd(0)
  {}

  kinetic::Result<Unit> start() {
    using ResultT = kinetic::Result<Unit>;

    _server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "socket() failed");
    }

    int opt = 1;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = inet_addr(_host.c_str());

    if (::bind(_server_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "bind() failed");
    }

    if (::listen(_server_fd, 1) < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "listen() failed");
    }

    return ResultT::ok({});
  }

  kinetic::Result<std::shared_ptr<TcpStream>> accept_client() {
    using ResultT = kinetic::Result<std::shared_ptr<TcpStream>>;

    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd = ::accept(_server_fd, (sockaddr *) &client_addr, &len);
    if (client_fd < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "accept() failed");
    }

    return ResultT::ok(std::shared_ptr<TcpStream>(new TcpStream(client_fd)));
  }
};

class TcpClient {
private:
  std::string _host;

  u16 _port;

  int _client_fd;

public:
  TcpClient(const std::string & host, const u16 port)
    : _host(host)
    , _port(port)
    , _client_fd(0)
  {}

  kinetic::Result<std::shared_ptr<TcpStream>> connect() {
    using ResultT = kinetic::Result<std::shared_ptr<TcpStream>>;

    _client_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_client_fd < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "socket() failed");
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = inet_addr(_host.c_str());

    if (::connect(_client_fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
      return ResultT::err(ErrorKind::ValueInvalid, "connect() failed");
    }

    return ResultT::ok(std::make_shared<TcpStream>(_client_fd));
  }
};

}
