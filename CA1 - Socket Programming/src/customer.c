#include "customer.h"

void customer_signin(Customer_Info *customer, struct sockaddr_in *addrOut, unsigned short udp_port)
{
    char buffer[INP_BUFF_SIZE];
    customer->udp_port = udp_port;
    customer->udp_fd = connect_udp(customer->udp_port);
    customer->tcp_port = generate_tcp_prot();
    customer->tcp_fd_server = connect_tcp_server(customer->tcp_port);
    customer->name = get_username(customer->udp_port, customer->tcp_port, Customer, customer->tcp_fd_server);
    customer->menu = parse_food_names();

    create_file(customer->name);
    char log_msg[INP_BUFF_SIZE];
    sprintf(log_msg, SIGNIN_LOG, customer->name);
    log_write(customer->name, log_msg);

    char* welcome_prompt = create_welcome_prompt(customer->name, Customer);
    write(1, welcome_prompt, strlen(welcome_prompt));
}

void show_menu(char** menu)
{
    int i = 0;
    while(menu[i] != NULL)
    {
        char* result;
        result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(result, "%d: %s\n", i+1,menu[i]);
        result = (char*)realloc(result, strlen(result) + 1);
        write(1,result,strlen(result));
        i++;
    }
}

void customer_tcp_handler(Customer_Info *customer, Msg msg)
{
    if(!strcmp(msg.detail ,RESTAURANT_INTRUDUCE_ITSELF))
    {
        char* result;
        result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(result, "%s / %d\n", msg.client_username, msg.client_tcp_port);
        result = (char*)realloc(result, strlen(result) + 1);
        write(1,result,strlen(result));
    }
    else if(!strcmp(msg.detail , ACCEPTS_ORDER))
    {
        char* result;
        result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(result, "%s accepted and your food is ready!\n", msg.client_username);
        result = (char*)realloc(result, strlen(result) + 1);
        write(1,result,strlen(result));

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, ACCEPTS_FOOD_ORDER_LOG, msg.client_username, msg.client_tcp_port, customer->name);
        log_write(customer->name, log_msg);
    }
    else if(!strcmp(msg.detail , REJECTS_ORDER))
    {
        char* result;
        result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(result, "%s rejected and cry about it retard!\n", msg.client_username);
        result = (char*)realloc(result, strlen(result) + 1);
        write(1,result,strlen(result));

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, REJECT_FOOD_ORDER_LOG, msg.client_username, msg.client_tcp_port, customer->name);
        log_write(customer->name, log_msg);
    }
}

void order_food(Customer_Info *customer, char* food_name, unsigned short port)
{
    Msg sending_msg;
    sending_msg.type = Customer;
    sending_msg.client_tcp_port = customer->tcp_port;
    sending_msg.client_username = customer->name;
    char* result;
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "order/food/%s/%s", customer->name, food_name);
    result = (char*)realloc(result, strlen(result) + 1);
    sending_msg.detail = result;
    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(port);
    send_tcp_msg(fd, sending_msg_str, 10);
    write(1,WAITING_FOR_RESTAURANT,strlen(WAITING_FOR_RESTAURANT));

    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    freeze_cmd();
    alarm(CUSTOMER_WAITING_TIME);
    int tcp_fd = accept_tcp_client(customer->tcp_fd_server);
    if(tcp_fd >= 0)
    {
        char *msg_str = receive_tcp(tcp_fd);
        Msg msg = decode_msg(msg_str);
        if (!strcmp(msg.detail , ACCEPTS_ORDER) || !strcmp(msg.detail , REJECTS_ORDER))
        {
            customer_tcp_handler(customer, msg);
        }
        else
            tcp_fd = -1;
    }
    alarm(0);
    active_cmd();
    if (tcp_fd < 0)
    {
        write(1,TIME_OUT_MSG, strlen(TIME_OUT_MSG));
        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, TIMEOUT_FOOD_ORDER_LOG, customer->name);
        log_write(customer->name, log_msg);
    }
}

void customer_command_handler(char* command, Customer_Info *customer)
{
    if(!strcmp(command, SHOW_RESTAURANTS_PROMPT))
    {
        Msg msg;
        msg.client_tcp_port = customer->tcp_port;
        msg.client_username = customer->name;
        msg.detail = SHOW_RESTAURANT_MSG_DETAIL;
        msg.type = Customer;
        char* msg_str = encode_msg(msg);
        broadcast_msg(customer->udp_port, msg_str);

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, SHOW_RESTAURANTS_LOG, customer->name);
        log_write(customer->name, log_msg);
    }
    else if(!strcmp(command, SHOW_MENU_PROMPT))
    {
        show_menu(customer->menu);
    }
    else if(!strcmp(command, ORDER_FOOD_PROMPT))
    {
        write(1, ENTER_RESTAURANT_PORT, strlen(ENTER_RESTAURANT_PORT));
        char* port;
        port = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        int bytes_read = read(STDIN_FILENO, port, INP_BUFF_SIZE);
        port[bytes_read - 1] = '\0';
        port = (char*)realloc(port, strlen(port) + 1);

        write(1, ENTER_FOOD_NAME, strlen(ENTER_FOOD_NAME));
        char* food_name;
        food_name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        bytes_read = read(STDIN_FILENO, food_name, INP_BUFF_SIZE);
        food_name[bytes_read - 1] = '\0';
        food_name = (char*)realloc(food_name, strlen(food_name) + 1);

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, ORDERED_FOOD_LOG, customer->name, food_name, port);
        log_write(customer->name, log_msg);

        order_food(customer, food_name, atoi(port));        
    }
}

void run(Customer_Info* customer, struct sockaddr_in* addrOut)
{
    fd_set master_set, working_set;
    int max_sd = customer->tcp_fd_server;
    FD_ZERO(&master_set);
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(customer->udp_fd, &master_set);
    FD_SET(customer->tcp_fd_server, &master_set);

    char buffer[INP_BUFF_SIZE] = {0};
    while (1)
    {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++)
        {
            if (FD_ISSET(i, &working_set))
            {
                memset(buffer, 0, INP_BUFF_SIZE);
                if (i == customer->udp_fd)
                {
                    char *msg_str = rcv_udp(customer->udp_fd);
                    Msg msg = decode_msg(msg_str);
                    udp_handler(&msg, customer->name, customer->tcp_port, Customer );
                }
                else if(i == customer->tcp_fd_server)
                {
                    int new_socket = accept_tcp_client(customer->tcp_fd_server);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }

                else if (i == STDIN_FILENO)
                {
                    int bytes_read = read(STDIN_FILENO, buffer, INP_BUFF_SIZE);
                    buffer[bytes_read - 1] = '\0';
                    customer_command_handler(buffer, customer);
                }
                else
                {
                    char *msg_str = receive_tcp(i);
                    Msg msg = decode_msg(msg_str);
                    customer_tcp_handler(customer, msg);
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr_out;
    Customer_Info customer;
    customer_signin(&customer, &addr_out, atoi(argv[1]));
    run(&customer, &addr_out);
}