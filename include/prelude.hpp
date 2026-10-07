#pragma once

#define KINETIC_NAMESPACE kinetic

#include <cstdint>
#include <cassert>
#include <new>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <iostream>

#include <sys/types.h>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using c8  = char;
using c16 = char16_t;
using c32 = char32_t;

using usize = std::size_t;
using ssize = ssize_t;

using rune = i32; // like Go
