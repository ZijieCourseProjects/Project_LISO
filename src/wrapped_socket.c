//
// Created by Eric Zhao on 27/3/2022.
//


#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include "stdlib.h"
#include <netinet/in.h>

#define ECHO_PORT 9999

int sock, client_sock;
struct sockaddr_in addr, cli_addr;
socklen_t cli_size;

int close_socket(int _sock) {
  if (close(_sock)) {
    fprintf(stderr, "Failed closing socket.\n");
    return 1;
  }
  return 0;
}

int sock_init() {
  /* all networked programs must create a socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Failed creating socket.\n");
    return EXIT_FAILURE;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(ECHO_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  /* servers bind sockets to ports---notify the OS they accept connections */
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))) {
    close_socket(sock);
    fprintf(stderr, "Failed binding socket.\n");
    return EXIT_FAILURE;
  }

  if (listen(sock, 5)) {
    close_socket(sock);
    fprintf(stderr, "Error listening on socket.\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int send_byte(char *buffer, int length) {
  if (send(client_sock, buffer, length, 0) != length) {
    close_socket(client_sock);
    close_socket(sock);
    fprintf(stderr, "Error sending to client.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int socket_accept() {
  cli_size = sizeof(cli_addr);
  if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                            &cli_size)) == -1) {
    close(sock);
    fprintf(stderr, "Error accepting connection.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int socket_receive(char *buff,int buff_size){
  int readret = recv(client_sock,buff,buff_size,0);

  if(readret == -1){
    close_socket(client_sock);
    close_socket(sock);
    fprintf(stderr, "Error reading from client socket.\n");
  }

  return readret;
}

int close_client() {
  if (close_socket(client_sock)) {
    close_socket(sock);
    fprintf(stderr, "Error closing client socket.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void socket_destroy(){
  close_socket(sock);
}

