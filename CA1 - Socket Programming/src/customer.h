#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include "udp.h"
#include "tcp.h"
#include "consts.h"
#include "customer.h"
#include "utils.h"
#include "JSON_parser.h"
#include "logger.h"

typedef struct Customer_Info{
    char* name;
    unsigned short int udp_port;
    unsigned short int tcp_port;
    int tcp_fd_client;
    int tcp_fd_server;
    int udp_fd;
    char** menu;
}Customer_Info;

void customer_signin(Customer_Info *customer, struct sockaddr_in *addrOut, unsigned short udp_port);

void show_menu(char** menu);

void customer_tcp_handler(Customer_Info *customer, Msg msg);

void order_food(Customer_Info *customer, char* food_name, unsigned short port);

void customer_command_handler(char* command, Customer_Info *customer);

#endif