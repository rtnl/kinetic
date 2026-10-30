#pragma once

#include "../_prelude.hpp"
#include "./Read.hpp"
#include "./Write.hpp"

namespace KINETIC_NAMESPACE {

class Stream
  : public Read
  , public Write
{};

}
