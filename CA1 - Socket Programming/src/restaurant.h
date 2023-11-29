#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <signal.h>
#include <time.h>
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
#include "logger.h"


typedef enum Status 
{ 
    Closed,
    Open 
}Status;

typedef struct Food_Order{
    char* name;
    char* food_name;
    unsigned short customer_tcp_port;
    Order_Status status;
    time_t generated_time;
}Food_Order;


typedef struct Restaurant_Info{
    char* name;
    unsigned short int udp_port;
    unsigned short int tcp_port;
    int tcp_fd_client;
    int tcp_fd_server;
    int udp_fd;
    ingredient* ingredients;
    int ingredients_counter;
    Recipes recipes;
    Status status;
    Food_Order* orders;
    int order_counter;
}Restaurant_Info;

void restaurant_signin(Restaurant_Info *restaurant, struct sockaddr_in *addrOut, unsigned short udp_port);
void show_recipes(Recipes recipes);
void show_ingredients(Restaurant_Info *restaurant);
void show_orders(Restaurant_Info *restaurant);
void show_suppliers(Restaurant_Info* restaurant);
void start_working(Restaurant_Info* restaurant);
void stop_working(Restaurant_Info* restaurant);
int find_order_index(Restaurant_Info* restaurant, unsigned short customer_port);
int find_food_index(Restaurant_Info* restaurant, int order_index);
void reject_order(Restaurant_Info* restaurant, unsigned short customer_port);
int have_enough_ingredients(Restaurant_Info* restaurant, int food_index);
void not_enough_ingredients(Restaurant_Info* restaurant, unsigned short customer_port);
void send_accept_msg(Restaurant_Info* restaurant, unsigned short customer_port);
void decrease_ingredients(Restaurant_Info* restaurant, int food_index);
void print_supplier(Msg* msg);
void get_order(Restaurant_Info* restaurant, ParsedLine* parsed_detail, unsigned short client_tcp_port);
int find_ingredient_index(Restaurant_Info* restaurant, char* ingredient_name);
void add_ingredient(Restaurant_Info* restaurant, char* ingredient_name, int count);
void order_ingredient(Restaurant_Info* restaurant, unsigned short supplier_tcp_port, char* ingredient_name, int ingredient_count);
void restaurant_command_handler(char* command, Restaurant_Info* restaurant);
void restaurant_tcp_handler(Msg msg, Restaurant_Info *restaurant);
void run(Restaurant_Info* restaurant, struct sockaddr_in* addrOut);

#endif