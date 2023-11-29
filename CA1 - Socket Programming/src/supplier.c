#include "supplier.h"

void supplier_signin(Supplier_Info *supplier, struct sockaddr_in *addrOut, unsigned short udp_port)
{
    char buffer[INP_BUFF_SIZE];
    supplier->udp_port = udp_port;
    supplier->udp_fd = connect_udp(supplier->udp_port);
    supplier->tcp_port = generate_tcp_prot();
    supplier->tcp_fd_server = connect_tcp_server(supplier->tcp_port);
    supplier->name = get_username(supplier->udp_port, supplier->tcp_port, Supplier, supplier->tcp_fd_server);
    supplier->ingredients = parse_ingridients();
    supplier->status = Free;

    create_file(supplier->name);
    char log_msg[INP_BUFF_SIZE];
    sprintf(log_msg, SIGNIN_LOG, supplier->name);
    log_write(supplier->name, log_msg);

    char* welcome_prompt = create_welcome_prompt(supplier->name, Supplier);
    write(1, welcome_prompt, strlen(welcome_prompt));
}

void adjust_order_status(Ingredient_Order* order)
{
    time_t ccurrent_time;
    time(&ccurrent_time);
    if(difftime(ccurrent_time,order->generated_time) >= 90.0 && order->status == Wating)
    {
        order->status = TimeOut;
    }
}

int have_ingredient(Supplier_Info *supplier)
{
    int i = 0;
    while(supplier->ingredients[i] != NULL)
    {
        if(!strcmp(supplier->ingredients[i], supplier->order.ingredient_name))
            return 1;
        i++;
    }
    return 0;
}

void get_order(Supplier_Info* supplier, ParsedLine* parsed_detail, Msg* msg)
{
    write(1, NEW_ORDER_MSG, strlen(NEW_ORDER_MSG));

    supplier->order.restaurant_tcp_port = msg->client_tcp_port;

    supplier->order.ingredient_name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(supplier->order.ingredient_name, "%s", parsed_detail->argv[2]);

    supplier->order.name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(supplier->order.name, "%s", msg->client_username);

    time(&supplier->order.generated_time);
    supplier->order.status = Wating;
    supplier->order.count = atoi(parsed_detail->argv[3]);
    supplier->status = Has_order;
}

void send_accept_msg(Supplier_Info *supplier)
{
    supplier->order.status = Accepted;
    Msg sending_msg;
    sending_msg.type = Supplier;
    sending_msg.client_tcp_port = supplier->tcp_port;
    sending_msg.client_username = supplier->name;
    sending_msg.detail = ACCEPTS_ORDER;

    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(supplier->order.restaurant_tcp_port);
    send_tcp_msg(fd, sending_msg_str, 10);
}

void reject_order(Supplier_Info *supplier)
{
    supplier->order.status = Rejected;
    Msg sending_msg;
    sending_msg.type = Supplier;
    sending_msg.client_tcp_port = supplier->tcp_port;
    sending_msg.client_username = supplier->name;
    sending_msg.detail = REJECTS_ORDER;

    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(supplier->order.restaurant_tcp_port);
    send_tcp_msg(fd, sending_msg_str, 10);
}

void reject_quickly(unsigned short supplier_port, char* supplier_name, unsigned short restaurant_port)
{
    Msg sending_msg;
    sending_msg.type = Supplier;
    sending_msg.client_tcp_port = supplier_port;
    sending_msg.client_username = supplier_name;
    sending_msg.detail = REJECTS_ORDER;

    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(restaurant_port);
    send_tcp_msg(fd, sending_msg_str, 10);
}

void supplier_command_handler(char* command, Supplier_Info *supplier)
{
    if(!strcmp(command, ANSWER_ORDER_PROMPT))
    {
        write(1, ANSWER_PROPMT, strlen(ANSWER_PROPMT));
        char answer[INP_BUFF_SIZE] = {0};
        int bytes_read = read(STDIN_FILENO, answer, INP_BUFF_SIZE);
        answer[bytes_read - 1] = '\0';

        if (supplier->status == Has_order)
        {
            adjust_order_status(&supplier->order);
            if(supplier->order.status == Accepted || supplier->order.status == Rejected)
            {
                write(1, ALREADY_ANSWERED_ORDER, strlen(ALREADY_ANSWERED_ORDER));
                return;
            }
            else if(supplier->order.status == TimeOut)
            {
                write(1, ALREADY_TIMEDOUT_ORDER, strlen(ALREADY_TIMEDOUT_ORDER));
                return;
            }
            else if(supplier->order.status == Wating)
            {
                if(!strcmp(answer,ACCEPTS_ORDER))
                {
                    if(!have_ingredient(supplier))
                    {
                        write(1, INVALID_INGREDIENT_MSG, strlen(INVALID_INGREDIENT_MSG));
                        reject_order(supplier);

                        char log_msg[INP_BUFF_SIZE];
                        sprintf(log_msg, REJECT_INGREDIENT_LOG, supplier->name, supplier->order.name, supplier->order.restaurant_tcp_port);
                        log_write(supplier->name, log_msg);
                    }
                    else
                    {
                        send_accept_msg(supplier);
                        char log_msg[INP_BUFF_SIZE];
                        sprintf(log_msg, ACCEPTED_INGREDIENT_LOG, supplier->name, supplier->order.name, supplier->order.restaurant_tcp_port);
                        log_write(supplier->name, log_msg);
                    }
                }
                if(!strcmp(answer,REJECTS_ORDER))
                {
                    reject_order(supplier);
                    char log_msg[INP_BUFF_SIZE];
                    sprintf(log_msg, REJECT_INGREDIENT_LOG, supplier->name, supplier->order.name, supplier->order.restaurant_tcp_port);
                    log_write(supplier->name, log_msg);
                }
            }
            supplier->status = Free;
        }
        else
            write(1, HAS_NO_ORDER_MSG, strlen(HAS_NO_ORDER_MSG));
    }
}

void supplier_tcp_handler(Msg msg, Supplier_Info* supplier)
{
    ParsedLine parsed_detail = custom_strtok(msg.detail,"/");
    if(supplier->status == Has_order)
    {
        adjust_order_status(&supplier->order);
    }
    if(!strcmp(parsed_detail.argv[0], "order") && !strcmp(parsed_detail.argv[1], "ingredient") )
    {
        if (supplier->status == Free || supplier->order.status != Wating)
        {
            get_order(supplier, &parsed_detail, &msg);
            char log_msg[INP_BUFF_SIZE];
            sprintf(log_msg, RECIEVE_ORDER, supplier->name, supplier->order.name, supplier->order.restaurant_tcp_port);
            log_write(supplier->name, log_msg);
        }
        else
        {
            reject_quickly(supplier->tcp_fd_server, supplier->name, msg.client_tcp_port);
            char log_msg[INP_BUFF_SIZE];
            sprintf(log_msg, REJECT_INGREDIENT_LOG, supplier->name, msg.client_username, msg.client_tcp_port);
            log_write(supplier->name, log_msg);
        }
    }
}

void run(Supplier_Info* supplier, struct sockaddr_in* addrOut)
{
    fd_set master_set, working_set;
    int max_sd = supplier->tcp_fd_server;
    FD_ZERO(&master_set);
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(supplier->udp_fd, &master_set);
    FD_SET(supplier->tcp_fd_server, &master_set);
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
                if (i == supplier->udp_fd)
                {
                    char *msg_str = rcv_udp(supplier->udp_fd);
                    Msg msg = decode_msg(msg_str);
                    udp_handler(&msg, supplier->name, supplier->tcp_port, Supplier);
                }
                else if(i == supplier->tcp_fd_server)
                {
                    int new_socket = accept_tcp_client(supplier->tcp_fd_server);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }

                else if (i == STDIN_FILENO)
                {
                    int bytes_read = read(STDIN_FILENO, buffer, INP_BUFF_SIZE);
                    buffer[bytes_read - 1] = '\0';
                    supplier_command_handler(buffer, supplier);
                }
                else
                {
                    char *msg_str = receive_tcp(i);
                    Msg msg = decode_msg(msg_str);
                    supplier_tcp_handler(msg, supplier);
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr_out;
    Supplier_Info supplier;
    supplier_signin(&supplier, &addr_out, atoi(argv[1]));
    run(&supplier,&addr_out);
}