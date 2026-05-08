#pragma once

#include "meta.h"
#include "type.h"
#include "uuid.h"
#include "tree.h"
#include "unit.h"
#include "error.h"
#include "either.h"
#include "option.h"
#include "result.h"
#include "slice.h"
#include "text.h"
#include "iterator.h"
#include "reader.h"
#include "utf8.h"
#include "node.h"
#include "parser.h"
#include "date.h"
#include "program.h"
#include "http.h"

namespace kinetic {
  static const std::string CommitHashRaw = KINETIC_STR(CABIN_KINETIC_COMMIT_HASH);
}
