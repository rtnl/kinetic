#pragma once

#include <cstdlib>

namespace kinetic {

template <typename I>
class Program {
private:

public:
  Program()
  {}

  void init(int argc, char **argv) {

  }

  virtual int run() = 0;
};

}

#define KINETIC_ENTRYPOINT(_PROGRAM) \
  int main(int argc, char **argv) { auto program = _PROGRAM(); program.init(argc, argv); return program.run(); }
