#pragma once
#include <_types.h>
#include <string>

struct ExternalServer {
  int32_t field_0;
  std::string field_4, field_8;
  int32_t field_C;
  bool_t isJava;

  ExternalServer();
  ExternalServer(const ExternalServer &);
  ExternalServer(int32_t, const std::string &, const std::string &, int32_t);
  ExternalServer(int32_t, const std::string &, const std::string &, int32_t,
                 bool_t);
  //~ExternalServer();
};
