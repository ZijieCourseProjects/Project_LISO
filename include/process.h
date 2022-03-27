//
// Created by Eric Zhao on 16/3/2022.
//

#ifndef INTERNETPROJECT_INCLUDE_PROCESS_H_
#define INTERNETPROJECT_INCLUDE_PROCESS_H_
#include "parse.h"


int process(Request * ptr_request,char* response);

struct Response_header{
  char type[64];
  char content[64];
};

struct Response_headers{
  int count;
  struct Response_header* headers;
};

void build_status_line(char* buf,char* status_with_reason);
void build_headers(char* buf,struct Response_headers * headers);

#endif //INTERNETPROJECT_INCLUDE_PROCESS_H_
