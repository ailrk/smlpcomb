#pragma once
#include "ast.h"
#include <memory>

template <typename T> inline std::unique_ptr<T> log_error(const char *str) {
  fprintf(stderr, "Error: %s\n", str);
  return nullptr;
}
