//
// Created by Eric Zhao on 12/4/2022.
//

#ifndef INTERNETPROJECT_SRC_CGI_H_
#define INTERNETPROJECT_SRC_CGI_H_
#include "parse.h"

int invoke_cgi_program(Request *ptr_request, char* response, char *file_path, char *query_message);

#endif //INTERNETPROJECT_SRC_CGI_H_
