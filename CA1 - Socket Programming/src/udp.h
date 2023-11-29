#ifndef BROADCAST_H_
#define BROADCAST_H

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
#include <asm-generic/socket.h>
#include "consts.h"

void broadcast_msg(unsigned short port, const char* msg);

int connect_udp(const int);

char* rcv_udp(int);

#endif