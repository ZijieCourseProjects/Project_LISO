//
// Created by Eric Zhao on 27/3/2022.
//

#ifndef INTERNETPROJECT_INCLUDE_LOGGER_H_
#define INTERNETPROJECT_INCLUDE_LOGGER_H_
#include "../include/parse.h"

int logger_init();
void logger_stop();
int log_access(Request *request,int response_code,int response_size);
int log_error(Request *request, char* err_level,char* err_message);

#endif //INTERNETPROJECT_INCLUDE_LOGGER_H_
