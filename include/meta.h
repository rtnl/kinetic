#pragma once

#define KINETIC_GETTER(type, field, name) \
  type get_##name() const { return field; }

#define KINETIC_SETTER(type, field, name) \
  void set_##name(type) { field = name; }
