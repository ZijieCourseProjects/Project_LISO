//
// Created by Eric Zhao on 27/3/2022.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "stdlib.h"
#include "time.h"
#include "sys/types.h"
#include "../include/parse.h"
#include "../include/wrapped_socket.h"

FILE* access_log;
FILE* error_log;
time_t timer;

int logger_init(){
  char fileName[32];
  memset(fileName,0,32);
  time(&timer);
  struct tm* now_time = localtime(&timer);
  sprintf(fileName,"access_%d_%d_%d.log",now_time->tm_year+1900,now_time->tm_mon,now_time->tm_mday);
  access_log = fopen(fileName, "a+");
  if(access_log == NULL){
    fprintf(stderr,"Cannot R/W AccessLogFile\n");
    return EXIT_FAILURE;
  }
  sprintf(fileName,"error_%d_%d_%d.log",now_time->tm_year+1900,now_time->tm_mon,now_time->tm_mday);
  error_log = fopen(fileName, "a+");
  if(error_log == NULL){
    fprintf(stderr,"Cannot R/W ErrorLogFile\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void logger_stop(){
  fclose(access_log);
  fclose(error_log);
}

int log_access(Request *request,int response_code,int response_size){
  char log[128];
  memset(log,0,128);
  struct tm* now_time = localtime(&timer);
  if(request == NULL){
    sprintf(log,"%s -- [%d/%d/%d %d:%d:%d] \"%s %s %s\" %d %d\n",get_client_ip(request->socket_fd),now_time->tm_mon,now_time->tm_mday,
            now_time->tm_year+1900,now_time->tm_hour,now_time->tm_min,now_time->tm_sec,
            "BAD REQUEST","","",response_code,response_size);
  }else{
    sprintf(log,"%s -- [%d/%d/%d %d:%d:%d] \"%s %s %s\" %d %d\n",get_client_ip(request->socket_fd),now_time->tm_mon,now_time->tm_mday,
            now_time->tm_year+1900,now_time->tm_hour,now_time->tm_min,now_time->tm_sec,
            request->http_version,request->http_uri,request->http_method,response_code,response_size);
  }
  if(fwrite(log, strlen(log), 1, access_log)==1){
    fflush(access_log);
    return EXIT_SUCCESS;
  }else{
    return EXIT_FAILURE;
  }
}

int log_error(Request *request, char* err_level,char* err_message){
  char log[128];
  memset(log,0,128);
  struct tm* now_time = localtime(&timer);
  sprintf(log,"%s -- [%d/%d/%d %d:%d:%d] [%s] \n%s\n",get_client_ip(request->socket_fd),now_time->tm_mon,now_time->tm_mday,
          now_time->tm_year+1900,now_time->tm_hour,now_time->tm_min,now_time->tm_sec,
          err_level,err_message);

  if(fwrite(log, strlen(log), 1, error_log)==1){
    fflush(error_log);
    return EXIT_SUCCESS;
  }else{
    return EXIT_FAILURE;
  }
}

