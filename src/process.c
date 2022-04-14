//
// Created by Eric Zhao on 16/3/2022.
//

#include "../include/parse.h"
#include "../include/process.h"
#include "unistd.h"
#include "sys/stat.h"
#include "../include/error_message.h"
#include "../include/logger.h"
#include "../include/util.h"
#include "../include/cgi.h"
#include "errno.h"

int process_get(Request * ptr_request,char *response);
int process_post(Request * ptr_request,char *response);
int process_head(Request * ptr_request, char *response);

int process(Request* ptr_request,char *response){

  //400 BAD REQUEST
  if(ptr_request == NULL){
    build_status_line(response,RESPONSE_400);
    build_headers(response,NULL);
    log_access(ptr_request,400, strlen(response));
    return strlen(response);
  }

  char *method_name = ptr_request->http_method;

  //505 WRONG HTTP VERSION
  if(strcmp(ptr_request->http_version,HTTP_VERSION)!=0){
    build_status_line(response,RESPONSE_505);
    build_headers(response, NULL);
    log_access(ptr_request,505, strlen(response));
    return strlen(response);
  }

  if(strcmp(method_name,"GET") == 0){
    return process_get(ptr_request,response);
  }else if (strcmp(method_name,"POST") == 0){
    return process_post(ptr_request,response);
  }else if (strcmp(method_name,"HEAD") == 0){
    return process_head(ptr_request,response);
  }

  //501 NOT IMPLEMENT
  build_status_line(response,RESPONSE_501);
  build_headers(response,NULL);
  log_access(ptr_request,501, strlen(response));
  return strlen(response);
}

int process_get(Request* ptr_request,char* response){
  char path[1024];
  memset(path,0,1024);

  getcwd(path,1024);
  strcat(path,"/static_site");
  if(strcmp(ptr_request->http_uri,"/") == 0){
    strcat(path,"/index.html");
  }else{
    strcat(path,ptr_request->http_uri);
  }
  char *cgi_message;
  if((cgi_message = strstr(path,"?")) != NULL){
    *cgi_message = '\0';
    cgi_message++;
    return invoke_cgi_program(ptr_request,response,path,cgi_message);
  }

  errno = 0;
  FILE *fp = fopen(path,"rb");

  if(fp == NULL){
    build_status_line(response,RESPONSE_404);
    build_headers(response, NULL);
    log_error(ptr_request,"error", strerror(errno));
    return strlen(response);
  }

  fseek( fp , 0L , SEEK_END);
  long lSize = ftell( fp );
  rewind( fp );
  char *buffer = malloc( lSize+1 );


  if(fread( buffer , lSize, 1 , fp)!= 1){
    fclose(fp);
    free(buffer);
    fputs("entire read fails",stderr);
    exit(1);
  }

  build_status_line(response,RESPONSE_200);

  struct Response_headers* ptr_response_headers = malloc(sizeof(struct Response_headers));

  ptr_response_headers->count=1;
  ptr_response_headers->headers = malloc(sizeof(struct Response_header)*ptr_response_headers->count);

  strcpy(ptr_response_headers->headers[0].type, "Content-Type");
  strcpy(ptr_response_headers->headers[0].content, "text/html");

  build_headers(response, ptr_response_headers);


  free(ptr_response_headers);

  memcpy(response+strlen(response),buffer, lSize);
  fclose(fp);
  free(buffer);
  log_access(ptr_request,200, strlen(response));
  return strlen(response);
}
int process_post(Request* ptr_request,char* response){
  return -2;
}
int process_head(Request* ptr_request, char* response){
  char path[1024];
  memset(path,0,1024);

  getcwd(path,1024);
  strcat(path,"/static_site");
  if(strcmp(ptr_request->http_uri,"/") == 0){
    strcat(path,"/index.html");
  }else{
    strcat(path,ptr_request->http_uri);
  }

  FILE *fp = fopen(path,"rb");

  if(fp == NULL){
    build_status_line(response,RESPONSE_404);
    build_headers(response, NULL);
    log_error(ptr_request,"error", strerror(errno));
    return strlen(response);
  }

  fclose(fp);

  build_status_line(response,RESPONSE_200);

  struct Response_headers* ptr_response_headers = malloc(sizeof(struct Response_headers));

  ptr_response_headers->count=1;
  ptr_response_headers->headers = malloc(sizeof(struct Response_header)*ptr_response_headers->count);

  strcpy(ptr_response_headers->headers[0].type, "Content-Type");
  strcpy(ptr_response_headers->headers[0].content, "text/html");

  build_headers(response, ptr_response_headers);

  free(ptr_response_headers);
  log_access(ptr_request,200, strlen(response));
  return strlen(response);
}

void build_status_line(char* buf,char* status_with_reason){
  //Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
  strcat(buf,HTTP_VERSION);
  strcat(buf," ");
  strcat(buf,status_with_reason);
  strcat(buf,CRLF);
}

void build_headers(char* buf,struct Response_headers * headers){
  if(headers == NULL){
    strcat(buf,CRLF);
    return;
  }

  for (int i = 0; i < headers->count; ++i) {
    strcat(buf,headers->headers[i].type);
    strcat(buf,": ");
    strcat(buf,headers->headers[i].content);
    strcat(buf,CRLF);
  }
  strcat(buf,CRLF);
}

