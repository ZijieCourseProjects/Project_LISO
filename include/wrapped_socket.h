//
// Created by Eric Zhao on 27/3/2022.
//

#ifndef INTERNETPROJECT_INCLUDE_WRAPPED_SOCKET_H_
#define INTERNETPROJECT_INCLUDE_WRAPPED_SOCKET_H_

int sock_init();
int send_byte(char *buffer, int length, int sockfd);
int socket_accept();
int socket_destroy();
int close_socket(int _sock);
int close_client(int fd);
int socket_receive(char *buff,int buff_size,int sockfd);
int select_socket(int _timeout, int block);
char *get_client_ip(int fd);
char *get_client_port(int fd);

#endif //INTERNETPROJECT_INCLUDE_WRAPPED_SOCKET_H_
