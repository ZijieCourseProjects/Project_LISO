//
// Created by Eric Zhao on 16/3/2022.
//

#include "../include/parse.h"
#include "../include/process.h"

int process_get(Request * ptr_request);
int process_post(Request * ptr_request);
int process_head(Request * ptr_request);

int process(Request* ptr_request){
  char *method_name = ptr_request->http_method;
  if(strcmp(method_name,"GET") == 0){
    return process_get(ptr_request);
  }else if (strcmp(method_name,"POST") == 0){
    return process_post(ptr_request);
  }else if (strcmp(method_name,"HEAD") == 0){
    return process_head(ptr_request);
  }
  //TODO:ADD MORE PROCESS FUNCTION!!
  return -1;
}

int process_get(Request* ptr_request){
  return 1;
}
int process_post(Request* ptr_request){
  return 1;
}
int process_head(Request* ptr_request){
  return 1;
}
