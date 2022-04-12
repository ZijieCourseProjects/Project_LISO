//
// Created by Eric Zhao on 27/3/2022.
//


#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include "stdlib.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "wrapped_socket.h"
#include "constant.h"

int sock;
struct sockaddr_in addr;
socklen_t cli_size;

int clients_fd[MAX_CONNECTION_NUMBER];
struct sockaddr_in client_addr[MAX_CONNECTION_NUMBER];

int client_count = 0;

int close_socket(int _sock) {
  if (close(_sock)) {
    fprintf(stderr, "Failed closing socket.\n");
    return 1;
  }
  return 0;
}

int add_client(int _sock, struct sockaddr_in _addr) {
  if (client_count >= MAX_CONNECTION_NUMBER) {
    fprintf(stderr, "Too many clients.\n");
    return 1;
  }
  clients_fd[_sock] = _sock;
  client_addr[_sock] = _addr;
  client_count++;
  return 0;
}

int remove_client(int _sock) {
  if (client_count <= 0) {
    fprintf(stderr, "No clients.\n");
    return 1;
  }
  if (close_socket(clients_fd[_sock])) {
    return 1;
  }
  clients_fd[_sock] = -1;
  client_count--;
  return 0;
}

/*
 * Selects the first available socket from the set of sockets,
 * returns -2 if no socket is available,-1 if new client is connected,-3 if error occurs;
 */
int select_socket(int _timeout, int block) {
  struct timeval timeout;
  timeout.tv_sec = _timeout;
  timeout.tv_usec = 0;
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(sock, &read_fds);

  for (int i = 0; i < MAX_CONNECTION_NUMBER; ++i) {
    if (clients_fd[i] != 0) {
      FD_SET(clients_fd[i], &read_fds);
    }
  }

  int ret;
  if (block) {
    ret = select(sock + 1, &read_fds, NULL, NULL, NULL);
  } else {
    ret = select(sock + 1, &read_fds, NULL, NULL, &timeout);
  }
  if (ret < 0) {
    fprintf(stderr, "Failed selecting socket.\n");
    return -3;
  } else if (ret == 0) {
    fprintf(stderr, "Timeout.\n");
    return -3;
  }
  for (int i = 0; i < MAX_CONNECTION_NUMBER; ++i) {
    if (FD_ISSET(clients_fd[i], &read_fds) && clients_fd[i] != 0) {
      return i;
    }
  }
  if (FD_ISSET(sock, &read_fds)) {
    return -1;
  }

  return -2;
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

  if (listen(sock, MAX_CONNECTION_NUMBER)) {
    close_socket(sock);
    fprintf(stderr, "Error listening on socket.\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int send_byte(char *buffer, int length, int client_fd) {
  if (send(client_fd, buffer, length, 0) != length) {
    close_socket(client_fd);
    socket_destroy();
    fprintf(stderr, "Error sending to client.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/*
 * return client_fd if success, -1 if error occurs
 */
int socket_accept() {
  int client_fd;
  struct sockaddr_in cli_addr;
  cli_size = sizeof(cli_addr);

  if ((client_fd = accept(sock, (struct sockaddr *) &cli_addr,
                          &cli_size)) == -1) {
    socket_destroy();
    fprintf(stderr, "Error accepting connection.\n");
    return -1;
  }

  if (add_client(client_fd, cli_addr)) {
    close_socket(client_fd);
    socket_destroy();
    fprintf(stderr, "Error accepting connection.\n");
    return -1;
  }

  return client_fd;
}

int socket_receive(char *buff, int buff_size, int client_fd) {
  int readret = recv(client_fd, buff, buff_size, 0);

  if (readret == -1) {
    close_socket(client_fd);
    socket_destroy();
    fprintf(stderr, "Error reading from client socket.\n");
  }

  return readret;
}

int close_client(int client_fd) {
  remove_client(client_fd);
  if (close_socket(client_fd)) {
    socket_destroy();
    fprintf(stderr, "Error closing client socket.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int socket_destroy() {
  if (close_socket(sock)){
    return -1;
  }
  return 0;
}

char *get_client_ip(struct sockaddr_in client_addr) {
  return inet_ntoa(client_addr.sin_addr);
}

