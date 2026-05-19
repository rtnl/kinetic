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
