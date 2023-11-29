#ifndef UTILS_H
#define UTILS_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include "udp.h"
#include "tcp.h"
#include "consts.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int generate_tcp_prot();

void alarm_handler(int sig);

char* get_username(unsigned short udp_port, unsigned short tcp_port, Client type, int tcp_fd_server);

char *read_line(int fd, int limit);

ParsedLine custom_strtok(char* input_line, const char* delims);

Msg decode_msg(char *msg);

char* encode_msg(Msg msg_struct);

void udp_handler(Msg* msg, char* username, unsigned short host_tcp_port, Client type);

char* create_welcome_prompt(char* username, Client type);

void freeze_cmd();

void active_cmd();

#endif