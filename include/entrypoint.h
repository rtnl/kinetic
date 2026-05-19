#pragma once

#include <stdexcept>

#define __KINETIC_ENTRYPOINT_MAIN(_PROGRAM) \
  int main(int argc, char **argv) { auto program = _PROGRAM(); program.init(argc, argv); return program.run(); }

#define __KINETIC_ENTRYPOINT_TEST(_PROGRAM) \
  int main(int argc, char **argv) { throw std::runtime_error("unimplemented"); }

#ifdef KINETIC_TEST
#define KINETIC_ENTRYPOINT(_PROGRAM) __KINETIC_ENTRYPOINT_TEST(_PROGRAM)
#else
#define KINETIC_ENTRYPOINT(_PROGRAM) __KINETIC_ENTRYPOINT_MAIN(_PROGRAM)
#endif
