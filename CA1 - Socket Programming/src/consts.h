#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define INP_BUFF_SIZE 1024
#define IP_ADDR "192.168.27.255"
#define LOCAL_HOST_ADDR "127.0.0.1"
#define RECIPES_FILE "src/recipes.json"
#define ENTER_SUPPLIER_PORT "please enter supplier port: "
#define ENTER_CUSTOMER_PORT "please enter customer port: "
#define ENTER_RESTAURANT_PORT "please enter restaurant port: "
#define ENTER_INGREDIENT_NAME "please enter ingredients name: "
#define ENTER_FOOD_NAME "please enter food name: "
#define ENTER_INGREDIENT_COUNT "please enter ingredients amount: "
#define LOCAL_HOST_ADDR "127.0.0.1"
#define SHOW_RECIPES_PROMPT "show recipes"
#define SHOW_INGREDIENTS_PROMPT "show ingredients"
#define SHOW_SUPPLIERS_PROMPT "show suppliers"
#define SHOW_ORDERS_PROMPT "show orders"
#define SHOW_HISTORY_PROMPT "show history"
#define ANSWER_ORDER_PROMPT "answer order"
#define ORDER_INGREDIENT_PROMPT "order ingredient"
#define SHOW_RESTAURANTS_PROMPT "show restaurants"
#define SHOW_MENU_PROMPT "show menu"
#define ORDER_FOOD_PROMPT "order food"
#define SIGNIN_PROMPT "Please enter username: "
#define INVALID_SIGNIN_PROMPT "Please enter another username: "
#define ANSWER_PROPMT "enter your answer: "
#define START_WORKING "start working"
#define STOP_WORKING "break"
#define RESTAURANT_ALREADY_OPEN "restaurant is already open!\n"
#define RESTAURANT_ALREADY_CLOSED "restaurant is already closed!\n"
#define OPENED_RESTAURANT_MSG "%s restaurant is open!\n"
#define CLOSED_RESTAURANT_MSG "%s restaurant is closed!\n"
#define NOT_ENOUGH_INGREDIENTS_MSG "you don't have enough ingredients!\n"
#define NEW_ORDER_MSG "new order!\n"
#define WAITING_FOR_RESTAURANT "waiting for answer from restaurant!\n"
#define WAITING_FOR_SUPPLIER "waiting for answer from supplier!\n"
#define RESTAURANT_INTRUDUCE_ITSELF "introduce_restaurant"
#define SUPPLIER_INTRUDUCE_ITSELF "introduce_supplier"
#define ACCEPTS_ORDER "accept"
#define REJECTS_ORDER "reject"
#define CUSTOMER_WAITING_TIME 120
#define RESTAURANT_WAITING_TIME 90
#define TIME_OUT_MSG "Time out!\n"
#define SHOW_RESTAURANT_MSG_DETAIL "look_for_restaurants"
#define SHOW_SUPPLIER_MSG_DETAIL "look_for_suppliers"
#define ALREADY_ANSWERED_ORDER "this order is already answered!\n"
#define ALREADY_TIMEDOUT_ORDER "this order is already answered!\n"
#define INVALID_INGREDIENT_MSG "invalid ingredient!\n"
#define HAS_NO_ORDER_MSG "you have no order!\n"
#define PORT_NOT_ENTERED_ERROR "please enter your port"
#define INVALID_USERNAME_ERROR "invalid username"
#define CHECK_USERNAME_COMMAND "check_username"
#define READ_WRITE_FILE_FORMAT 0666



#define SIGNIN_LOG "%s signed in."
#define SHOW_RESTAURANTS_LOG "%s asked restaurants to introduce themselves."
#define ORDERED_FOOD_LOG "%s ordered %s to %s."
#define ACCEPTS_FOOD_ORDER_LOG "%s(%d) accepted %s food request."
#define REJECT_FOOD_ORDER_LOG "%s(%d) rejected %s food request."
#define TIMEOUT_FOOD_ORDER_LOG "%s food request timed out."
#define REJECT_INGREDIENT_LOG "%s rejected %s(%d) ingredient request."
#define ACCEPTED_INGREDIENT_LOG "%s accepted %s(%d) ingredient request."
#define RECIEVE_ORDER "%s recieved order from %s(%d)."
#define START_WORKING_LOG "%s started working"
#define STOP_WORKING_LOG "%s stopped working"
#define SHOW_SUPPLIERS_LOG "%s asked suppliers to introduce themselves."
#define REJECT_FOOD_LOG "%s rejected %s(%d) food request."
#define ACCEPTED_FOOD_LOG "%s accepted %s(%d) food request."
#define ORDERED_INGREDIENT_LOG "%s ordered %s to %d."
#define TIMEOUT_INGREDIENT_ORDER_LOG "%s ingredient request timed out."

typedef enum Client
{
    Customer,
    Restaurant,
    Supplier
} Client;

typedef struct ParsedLine
{
    int count;
    char **argv;
} ParsedLine;

typedef struct Msg
{
    char *client_username;
    unsigned short client_tcp_port;
    Client type;
    char *detail;
} Msg;

typedef enum Order_Status { 
    Accepted,
    Rejected,
    TimeOut,
    Wating 
}Order_Status;

#endif