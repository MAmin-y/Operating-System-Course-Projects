#ifndef SUPPLIER_H
#define SUPPLIER_H

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
#include <sys/types.h>
#include "utils.h"
#include "JSON_parser.h"
#include <time.h>
#include "logger.h"

typedef enum Supplier_Status { 
    Has_order,
    Free
} Supplier_Status;

typedef struct Ingredient_Order{
    char* name;
    char* ingredient_name;
    int count;
    unsigned short restaurant_tcp_port;
    Order_Status status;
    time_t generated_time;
}Ingredient_Order;

typedef struct Supplier_Info{
    char* name;
    unsigned short int udp_port;
    unsigned short int tcp_port;
    int tcp_fd_client;
    int tcp_fd_server;
    int udp_fd;
    char** ingredients;
    Supplier_Status status;
    Ingredient_Order order;
}Supplier_Info;

void supplier_signin(Supplier_Info *supplier, struct sockaddr_in *addrOut, unsigned short udp_port);
void adjust_order_status(Ingredient_Order* order);
int have_ingredient(Supplier_Info *supplier);
void get_order(Supplier_Info* supplier, ParsedLine* parsed_detail, Msg* msg);
void send_accept_msg(Supplier_Info *supplier);
void reject_order(Supplier_Info *supplier);
void reject_quickly(unsigned short supplier_port, char* supplier_name, unsigned short restaurant_port);
void supplier_command_handler(char* command, Supplier_Info *supplier);
void supplier_tcp_handler(Msg msg, Supplier_Info* supplier);

#endif