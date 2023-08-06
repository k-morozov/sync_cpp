#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define SYNC_EXPORT __declspec(dllexport)
#else
  #define SYNC_EXPORT
#endif

SYNC_EXPORT void sync();
SYNC_EXPORT void sync_print_vector(const std::vector<std::string> &strings);
