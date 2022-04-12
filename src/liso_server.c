/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../include/process.h"
#include "../include/wrapped_socket.h"
#include "../include/error_message.h"
#include "../include/logger.h"

#define BUF_SIZE 1024*1024

int main(int argc, char *argv[]) {
  logger_init();
  int readret;
  char buf[BUF_SIZE];

  if (sock_init() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  printf(" _    _           ___ _            _          _ _ _\n"
         "| |  (_)___ ___  / __| |_ __ _ _ _| |_ ___ __| | | |\n"
         "| |__| (_-</ _ \\ \\__ \\  _/ _` | '_|  _/ -_) _` |_|_|\n"
         "|____|_/__/\\___/ |___/\\__\\__,_|_|  \\__\\___\\__,_(_|_)\n");

  /* finally, loop waiting for input and then write it back */
  while (1) {

    readret = 0;

    int fd = select_socket(0,1);

    if(fd == -1){
      if((fd = socket_accept())  == -1){
        socket_destroy();
        return EXIT_FAILURE;
      }
    }

    char response[BUF_SIZE];
    readret = socket_receive(buf,BUF_SIZE, fd);

    if(readret < 1){
      socket_destroy();
      close_socket(fd);
      continue;
    }

    char* nextRequest = buf;
    while(1){
      Request *requst = parse(nextRequest, BUF_SIZE, 0);
      int response_size;
      response_size = process(requst,response);
      switch (response_size) {
        case -2:
          if (send_byte(buf, readret,fd) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
          }
          break;
        default:
          if (send_byte(response, response_size, fd) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
          }
      }
      if(requst!=NULL){
        free(requst->headers);
        free(requst);
      }
      if((nextRequest = strstr(nextRequest,"\r\n\r\n"))==NULL){
        break;
      }else{
        nextRequest+=4;
        if(*nextRequest == 0 || strcmp(nextRequest,"\r\n")==0){
          break;
        }
      }
      memset(response, 0, BUF_SIZE);
    }
    memset(buf, 0, BUF_SIZE);

  }
  socket_destroy();

  return EXIT_SUCCESS;
}
