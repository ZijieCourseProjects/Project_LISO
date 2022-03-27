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

#define BUF_SIZE 8192

int main(int argc, char *argv[]) {
  logger_init();
  int readret;
  char buf[BUF_SIZE];

  if (sock_init() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  /* finally, loop waiting for input and then write it back */
  while (1) {

    if(socket_accept()!=EXIT_SUCCESS){
      return EXIT_FAILURE;
    }

    readret = 0;

    char response[BUF_SIZE];
    while ((readret = socket_receive(buf,BUF_SIZE)) >= 1) {
      Request *requst = parse(buf, BUF_SIZE, 0);
      int response_size;
      response_size = process(requst,response);

      switch (response_size) {
        case -2:
          if (send_byte(buf, readret) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
          }
          break;
        default:
          if (send_byte(response, response_size) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
          }
      }
      if(requst!=NULL){
        free(requst->headers);
        free(requst);
      }
      memset(buf, 0, BUF_SIZE);
      memset(response, 0, BUF_SIZE);
    }

    if (close_client() != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }
  }

  socket_destroy();

  return EXIT_SUCCESS;
}
