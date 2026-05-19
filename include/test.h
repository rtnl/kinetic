#pragma once

#include <./type.h>
#include <./program.h>
#include <stdexcept>

namespace kinetic {

class TestProgram : kinetic::Program<u8> {
public:
  int run() override {
    throw std::runtime_error("unimplemented");
  }
};

}
