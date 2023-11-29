#ifndef TCP_H
#define TCP_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "consts.h"

int connect_tcp_server(int);

int accept_tcp_client(int);

int connect_tcp_client(int);

int send_tcp_msg(int, const char*, int);

char* receive_tcp(int);

#endif