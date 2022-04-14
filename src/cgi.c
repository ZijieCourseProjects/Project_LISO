//
// Created by Eric Zhao on 12/4/2022.
//

#include "../include/cgi.h"
#include "../include/constant.h"
#include "../include/process.h"
#include "../include/util.h"
#include "../include/logger.h"
#include "../include/error_message.h"
#include "../include/wrapped_socket.h"
#include "../include/parse.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"



void execve_error_handler(int error_code) {
    switch (error_code) {
        case E2BIG:
            printf("E2BIG: Argument list too long.\n");
            break;
        case EACCES:
            printf("EACCES: Permission denied.\n");
            break;
        case EFAULT:
            printf("EFAULT: Bad address.\n");
            break;
        case EINVAL:
            printf("EINVAL: Invalid argument.\n");
            break;
        case ENOENT:
            printf("ENOENT: File not found or path not accessible.\n");
            break;
        case ENOMEM:
            printf("ENOMEM: Cannot allocate memory.\n");
            break;
        case ENOTDIR:
            printf("ENOTDIR: Not a directory.\n");
            break;
        default:
            printf("Unknown error.\n");
            break;
    }
}

int invoke_cgi_program(Request *ptr_request, char* response,char *file_path, char *query_message){
  pid_t pid;
  int stdin_pipe[2];
  int stdout_pipe[2];
  char buf[CGI_BUF_SIZE];
  int readret;

  memset(buf, 0, CGI_BUF_SIZE);

  char* ARGV[]={file_path,NULL};
  char ENV_BUF[21][1024]={
    "CONTENT_LENGTH=",
    "CONTENT_TYPE=",
    "GATEWAY_INTERFACE=CGI/1.1",
    "PATH_INFO=",
    "QUERY_STRING=",
    "REMOTE_ADDR=",
    "REMOTE_METHOD=",
    "REMOTE_URI=",
    "SCRIPT_NAME=",
    "SERVER_PORT=",
    "SERVER_PROTOCOL=HTTP/1.1",
    "SERVER_SOFTWARE=Liso/1.0",
    "HTTP_ACCEPT=",
    "HTTP_REFERER=",
    "HTTP_ACCEPT_ENCODING=",
    "HTTP_ACCEPT_LANGUAGE=",
    "HTTP_ACCEPT_CHARSET=",
    "HTTP_HOST=",
    "HTTP_COOKIE=",
    "HTTP_USER_AGENT=",
    "HTTP_CONNECTION=",
  };

  char *env[]={ENV_BUF[0],ENV_BUF[1],ENV_BUF[2],ENV_BUF[3],ENV_BUF[4],ENV_BUF[5],ENV_BUF[6],ENV_BUF[7],ENV_BUF[8],ENV_BUF[9],ENV_BUF[10],ENV_BUF[11],ENV_BUF[12],ENV_BUF[13],ENV_BUF[14],ENV_BUF[15],ENV_BUF[16],ENV_BUF[17],ENV_BUF[18],ENV_BUF[19],ENV_BUF[20],NULL};

  my_strcat(ENV_BUF[0], get_header_value(ptr_request, "Content-Length"));
  my_strcat(ENV_BUF[1], get_header_value(ptr_request, "Content-Type"));
  my_strcat(ENV_BUF[4], query_message);
  my_strcat(ENV_BUF[5], get_client_ip(ptr_request->socket_fd));
  my_strcat(ENV_BUF[6], ptr_request->http_method);
  my_strcat(ENV_BUF[7], ptr_request->http_uri);
  my_strcat(ENV_BUF[8], get_filename(file_path));
  my_strcat(ENV_BUF[9], get_client_port(ptr_request->socket_fd));
  my_strcat(ENV_BUF[12], get_header_value(ptr_request, "Accept"));
  my_strcat(ENV_BUF[13], get_header_value(ptr_request, "Referer"));
  my_strcat(ENV_BUF[14], get_header_value(ptr_request, "Accept-Encoding"));
  my_strcat(ENV_BUF[15], get_header_value(ptr_request, "Accept-Language"));
  my_strcat(ENV_BUF[16], get_header_value(ptr_request, "Accept-Charset"));
  my_strcat(ENV_BUF[17], get_header_value(ptr_request, "Host"));
  my_strcat(ENV_BUF[18], get_header_value(ptr_request, "Cookie"));
  my_strcat(ENV_BUF[19], get_header_value(ptr_request, "User-Agent"));
  my_strcat(ENV_BUF[20], get_header_value(ptr_request, "Connection"));

  if (pipe(stdin_pipe) < 0) {
    printf("stdin pipe error\n");
    return -1;
  }
  if (pipe(stdout_pipe) < 0) {
    printf("stdout pipe error\n");
    return -1;
  }

  pid = fork();
  if (pid < 0) {
    printf("fork error\n");
    return -1;
  }

  //child process
  if (pid == 0) {
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);

    dup2(stdin_pipe[0], fileno(stdin));
    dup2(stdout_pipe[1], fileno(stdout));

    execve(file_path, ARGV, env);

    //Error occurs
    execve_error_handler(errno);
    printf("execvp error\n");
    return -1;
  }

  if (pid > 0) {
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);

    read(stdout_pipe[0], buf, CGI_BUF_SIZE);
    int result;
    wait(&result);

    if (result != 0) {
      struct Response_headers *header = malloc(sizeof(struct Response_headers));
      header->count = 1;
      header->headers = malloc(sizeof(struct Response_header));
      strcpy(header->headers[0].type,"Connection");
      strcpy(header->headers[0].content,"close");
      build_status_line(response,RESPONSE_500);
      build_headers(response, header);
      log_access(ptr_request,500, strlen(response));
      return strlen(response);
    }

    strcpy(response,buf);

    close(stdin_pipe[1]);
    close(stdout_pipe[0]);

    return strlen(response);

  }

}
