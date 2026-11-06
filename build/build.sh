#!/bin/sh
set -e

clang++                   \
  -std=c++11              \
  -g                      \
  -I ./include            \
  -DKINETIC_FEATURE_JSON  \
  -o ./build/kinetic      \
  ./src/main.cpp

clang++                   \
  -std=c++11              \
  -g                      \
  -I ./include            \
  -I ./test               \
  -DKINETIC_FEATURE_JSON  \
  -DKINETIC_TEST          \
  -DKINETIC_TEST_LIB      \
  -o ./build/kinetic_test \
  ./src/main.cpp
