#pragma once

#define KINETIC_GETTER(field, name) \
  auto get_##name() const -> decltype(field) { return field; }

#define KINETIC_SETTER(type, field, name) \
  void set_##name(type) { field = name; }
