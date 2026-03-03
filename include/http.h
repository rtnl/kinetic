#pragma once

#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netdb.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "meta.h"
#include "result.h"
#include "buffer.h"

namespace kinetic {

constexpr const char HttpCRLF[2] = {'\r', '\n'};

enum class HttpVersion {
  v1_1,
};

enum class HttpMethod {
  Get,
  Post,
  Put,
  Patch,
  Delete,
  Head,
  Options,
  Trace,
  Connect,
};

enum class HttpStatus {
  Ok,
};

class HttpRequest {
private:
  HttpVersion _version;

  std::string _host;

  std::string _target;

  HttpMethod _method;

  std::vector<std::string> _header;

  ByteBuffer _body;

public:
  HttpRequest(
    const HttpVersion   version,
    const std::string & host,
    const std::string & target,
    const HttpMethod    method)
    : _version(version)
    , _host(host)
    , _target(target)
    , _method(method)
    , _header({})
    , _body(ByteBuffer())
  {}

  KINETIC_GETTER(_version, version)

  KINETIC_GETTER(_host, host)

  KINETIC_GETTER(_target, target)

  KINETIC_GETTER(_method, method)

  const std::vector<std::string> & get_header() const {
    return _header;
  }

  void add_header(const std::string & value) {
    _header.emplace_back(value);
  }

  const ByteBuffer & get_body() const {
    return _body;
  }

  ByteBuffer * get_body_mut() {
    return &_body;
  }
};

class HttpResponse {
private:
  HttpStatus _status;

  std::vector<std::string> _header;

  ByteBuffer _body;

public:
  HttpResponse(const HttpStatus status)
    : _status(status)
    , _header({})
    , _body(ByteBuffer())
  {}

  KINETIC_GETTER(_status, status)

  const std::vector<std::string> & get_header() const {
    return _header;
  }

  const ByteBuffer & get_body() const {
    return _body;
  }

  ByteBuffer * get_body_mut() {
    return &_body;
  }
};

class HttpClient {
private:

public:
  HttpClient()
  {}

  static std::string get_version_str(const HttpVersion version) {
    switch (version) {
      case kinetic::HttpVersion::v1_1:
        return "HTTP/1.1";
      default:
        throw std::logic_error("unimplemented");
    }
  }

  static std::string get_method_str(const HttpMethod method) {
    switch (method) {
      case kinetic::HttpMethod::Get:
        return "GET";
      case kinetic::HttpMethod::Post:
        return "POST";
      case kinetic::HttpMethod::Put:
        return "PUT";
      case kinetic::HttpMethod::Patch:
        return "PATCH";
      case kinetic::HttpMethod::Delete:
        return "DELETE";
      case kinetic::HttpMethod::Head:
        return "HEAD";
      case kinetic::HttpMethod::Options:
        return "OPTIONS";
      case kinetic::HttpMethod::Trace:
        return "TRACE";
      case kinetic::HttpMethod::Connect:
        return "CONNECT";
      default:
        throw std::logic_error("unimplemented");
    }
  }

  kinetic::Result<HttpResponse> execute(const HttpRequest & req) { // Todo: async?
    auto out = ByteBuffer();

    // start
    out.write(get_method_str(req.get_method())); out.write(" ");
    out.write(req.get_target()); out.write(" ");
    out.write(get_version_str(req.get_version()));
    out.write(HttpCRLF, 2);
    // end start

    // header
    for (const std::string & it : req.get_header()) {
      out.write(it);
      out.write(HttpCRLF, 2);
    }
    // end header

    // body
    out.write(req.get_body().raw(), req.get_body().size());
    // end body

    out.write(HttpCRLF, 2);
    out.write(HttpCRLF, 2);

    const size_t out_buf_size = out.size();
    const u8 * out_buf = static_cast<const u8 *>(calloc(out_buf_size, sizeof(u8)));
    memcpy(const_cast<u8 *>(out_buf), out.raw(), out.size());

    addrinfo hints{}, *resi;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    const std::string port = "8000"; // Todo: take from request

    if (getaddrinfo(req.get_host().c_str(), port.c_str(), &hints, &resi) != 0) {
      throw std::runtime_error("failed to resolve host: " + req.get_host() + ":" + port); // Todo: return error
    }

    int sock = socket(resi->ai_family, resi->ai_socktype, resi->ai_protocol); // Todo: reuse socket
    if (sock < 0) {
      freeaddrinfo(resi);
      throw std::runtime_error("failed to create socket"); // Todo: return error
    }

    if (connect(sock, resi->ai_addr, resi->ai_addrlen) != 0) {
      close(sock);
      freeaddrinfo(resi);
      throw std::runtime_error("failed to connect host: " + req.get_host() + ":" + port); // Todo: return error
    }

    send(sock, out_buf, out_buf_size, 0);
    free((void *)(out_buf));

    auto inc = ByteBuffer(); // Todo: write directly into res.body

    u8 buffer[8192];
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
      buffer[bytes] = '\0';

      inc.write(buffer, bytes);

      break; // Todo: use response length
    }

    close(sock);
    freeaddrinfo(resi);

    HttpResponse res(HttpStatus::Ok); // Todo: parse from response
    // Todo: parse response header
    res.get_body_mut()->write(buffer, bytes);

    return Result<HttpResponse>::ok(res);
  }
};

}
