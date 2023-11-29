#include "restaurant.h"

void restaurant_signin(Restaurant_Info *restaurant, struct sockaddr_in *addrOut, unsigned short udp_port)
{
    char buffer[INP_BUFF_SIZE];
    restaurant->udp_port = udp_port;
    restaurant->udp_fd = connect_udp(restaurant->udp_port);
    restaurant->tcp_port = generate_tcp_prot();
    restaurant->tcp_fd_server = connect_tcp_server(restaurant->tcp_port);
    restaurant->name = get_username(restaurant->udp_port, restaurant->tcp_port, Restaurant, restaurant->tcp_fd_server);
    restaurant->status = Open;
    restaurant->recipes = get_foods();
    restaurant->order_counter = 0;
    restaurant->ingredients_counter = 0;

    create_file(restaurant->name);
    char log_msg[INP_BUFF_SIZE];
    sprintf(log_msg, SIGNIN_LOG, restaurant->name);
    log_write(restaurant->name, log_msg);

    char* welcome_prompt = create_welcome_prompt(restaurant->name, Restaurant);
    write(1,welcome_prompt,strlen(welcome_prompt));
}

void show_recipes(Recipes recipes)
{
    for(int i = 0; i < recipes.food_counter; i++)
    {
        char* result;
        result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(result, "%s:\n", recipes.foods[i].name);
        result = (char*)realloc(result, strlen(result) + 1);
        write(1,result,strlen(result));
        for(int j = 0; j < recipes.foods[i].ingredient_count; j++)
        {
            char* result2;
            result2 = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result2, "\t\t%s : %d\n", recipes.foods[i].ingredients[j].name, recipes.foods[i].ingredients[j].count);
            result2 = (char*)realloc(result2, strlen(result2) + 1);
            write(1,result2,strlen(result2));
        }
    }
}

void show_ingredients(Restaurant_Info *restaurant)
{
    for(int i = 0; i < restaurant->ingredients_counter; i++)
    {
        if(restaurant->ingredients[i].count != 0)
        {
            char* result;
            result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, "%s : %d\n", restaurant->ingredients[i].name, restaurant->ingredients[i].count);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
    }
}

void adjust_order_status(Food_Order* order)
{
    time_t ccurrent_time;
    time(&ccurrent_time);
    if(difftime(ccurrent_time,order->generated_time) >= 120.0 && order->status == Wating)
    {
        order->status = TimeOut;
    }
}

void show_orders(Restaurant_Info *restaurant)
{
    for(int i = 0; i < restaurant->order_counter; i++)
    {
        adjust_order_status(&restaurant->orders[i]);
        if(restaurant->orders[i].status == Wating)
        {
            char* result;
            result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, "%s / %d / %s\n", restaurant->orders[i].name, restaurant->orders[i].customer_tcp_port, restaurant->orders[i].food_name);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
    }
}

void show_sales_history(Restaurant_Info *restaurant)
{
    char* result;
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    for(int i = 0; i < restaurant->order_counter; i++)
    {
        adjust_order_status(&restaurant->orders[i]);
        if(restaurant->orders[i].status == Accepted)
        {
            sprintf(result, "%s / %s / accepted\n", restaurant->orders[i].name, restaurant->orders[i].food_name);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
        else if(restaurant->orders[i].status == Rejected)
        {
            sprintf(result, "%s / %s / rejected\n", restaurant->orders[i].name, restaurant->orders[i].food_name);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
        else if(restaurant->orders[i].status == TimeOut)
        {
            sprintf(result, "%s / %s / timed-out\n", restaurant->orders[i].name, restaurant->orders[i].food_name);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
    }
}

void show_suppliers(Restaurant_Info* restaurant)
{
    Msg msg;
    msg.client_tcp_port = restaurant->tcp_port;
    msg.client_username = restaurant->name;
    msg.detail = SHOW_SUPPLIER_MSG_DETAIL;
    msg.type = Restaurant;
    char* msg_str = encode_msg(msg);
    broadcast_msg(restaurant->udp_port, msg_str);

    char log_msg[INP_BUFF_SIZE];
    sprintf(log_msg, SHOW_SUPPLIERS_LOG, restaurant->name);
    log_write(restaurant->name, log_msg);
}

void start_working(Restaurant_Info* restaurant)
{
    if(restaurant->status == Open)
        write(1, RESTAURANT_ALREADY_OPEN, strlen(RESTAURANT_ALREADY_OPEN));
    else
    {
        restaurant->status = Open;
        Msg msg;
        msg.client_tcp_port = restaurant->tcp_port;
        msg.client_username = restaurant->name;
        msg.type = Restaurant;
        msg.detail = START_WORKING;
        char* msg_str = encode_msg(msg);
        broadcast_msg(restaurant->udp_port, msg_str);

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, START_WORKING_LOG, restaurant->name);
        log_write(restaurant->name, log_msg);
    }
}

void stop_working(Restaurant_Info* restaurant)
{
    if(restaurant->status == Closed)
        write(1, RESTAURANT_ALREADY_CLOSED, strlen(RESTAURANT_ALREADY_CLOSED));
    else
    {
        restaurant->status = Closed;
        Msg msg;
        msg.client_tcp_port = restaurant->tcp_port;
        msg.client_username = restaurant->name;
        msg.type = Restaurant;
        msg.detail = STOP_WORKING;
        char* msg_str = encode_msg(msg);
        broadcast_msg(restaurant->udp_port, msg_str);

        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, STOP_WORKING_LOG, restaurant->name);
        log_write(restaurant->name, log_msg);
    }
}

int find_order_index(Restaurant_Info* restaurant, unsigned short customer_port)
{
    int found_flag = 0;
    int order_index = 0;
    for (int i = 0; i < restaurant->order_counter; i++)
    {
        if(restaurant->orders[order_index].customer_tcp_port == customer_port)
        {
            found_flag = 1;
            order_index = i;
        }
    }
    if(found_flag)
        return order_index;
    else
        return -1;
}

int find_food_index(Restaurant_Info* restaurant, int order_index)
{
    int food_index = 0;
    for(food_index; food_index < restaurant->recipes.food_counter; food_index++)
    {
        if(!strcmp(restaurant->orders[order_index].food_name, restaurant->recipes.foods[food_index].name))
        {
            break;
        }
    }
    return food_index;
}

void reject_order(Restaurant_Info* restaurant, unsigned short customer_port)
{
    Msg sending_msg;
    sending_msg.type = Restaurant;
    sending_msg.client_tcp_port = restaurant->tcp_port;
    sending_msg.client_username = restaurant->name;
    sending_msg.detail = REJECTS_ORDER;
    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(customer_port);
    send_tcp_msg(fd, sending_msg_str, 10);
}

int have_enough_ingredients(Restaurant_Info* restaurant, int food_index)
{
    for(int k = 0; k < restaurant->recipes.foods[food_index].ingredient_count; k++)
    {
        int found = 0;
        for(int l = 0; l < restaurant->ingredients_counter; l++)
        {
            if(!strcmp(restaurant->recipes.foods[food_index].ingredients[k].name, restaurant->ingredients[l].name))
            {
                if(restaurant->recipes.foods[food_index].ingredients[k].count <= restaurant->ingredients[l].count)
                {
                    found = 1;
                    break;
                }
            }
        }
        if (!found) return 0;
    }
    return 1;
}

void not_enough_ingredients(Restaurant_Info* restaurant, unsigned short customer_port)
{
    write(1,NOT_ENOUGH_INGREDIENTS_MSG, strlen(NOT_ENOUGH_INGREDIENTS_MSG));
    reject_order(restaurant, customer_port);
}

void send_accept_msg(Restaurant_Info* restaurant, unsigned short customer_port)
{
    Msg sending_msg;
    sending_msg.type = Restaurant;
    sending_msg.client_tcp_port = restaurant->tcp_port;
    sending_msg.client_username = restaurant->name;
    sending_msg.detail = ACCEPTS_ORDER;

    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(customer_port);
    send_tcp_msg(fd, sending_msg_str, 10);
}

void decrease_ingredients(Restaurant_Info* restaurant, int food_index)
{
    for(int k = 0; k < restaurant->recipes.foods[food_index].ingredient_count; k++)
    {
        for(int l = 0; l < restaurant->ingredients_counter; l++)
        {
            if(!strcmp(restaurant->recipes.foods[food_index].ingredients[k].name, restaurant->ingredients[l].name))
                restaurant->ingredients[l].count -= restaurant->recipes.foods[food_index].ingredients[k].count;
        }
    }
}

void print_supplier(Msg* msg)
{
    char* result;
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "%s / %d\n", msg->client_username, msg->client_tcp_port);
    result = (char*)realloc(result, strlen(result));
    write(1,result,strlen(result));
}

void get_order(Restaurant_Info* restaurant, ParsedLine* parsed_detail, unsigned short client_tcp_port)
{
    write(1, NEW_ORDER_MSG, strlen(NEW_ORDER_MSG));
    if(restaurant->order_counter == 0)
        restaurant->orders = (Food_Order*)malloc( (restaurant->order_counter + 1) * sizeof(Food_Order));
    else
        restaurant->orders = (Food_Order*)realloc(restaurant->orders, (restaurant->order_counter + 1) * sizeof(Food_Order));

    restaurant->orders[restaurant->order_counter].customer_tcp_port = client_tcp_port;

    restaurant->orders[restaurant->order_counter].food_name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(restaurant->orders[restaurant->order_counter].food_name, "%s", parsed_detail->argv[3]);

    restaurant->orders[restaurant->order_counter].name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(restaurant->orders[restaurant->order_counter].name, "%s", parsed_detail->argv[2]);
    time(&restaurant->orders[restaurant->order_counter].generated_time);
    restaurant->orders[restaurant->order_counter].status = Wating;
    restaurant->order_counter++;
}

int find_ingredient_index(Restaurant_Info* restaurant, char* ingredient_name)
{
    for (int ingredient_index = 0; ingredient_index < restaurant->ingredients_counter; ingredient_index++)
    {
        if (!strcmp(ingredient_name, restaurant->ingredients[ingredient_index].name))
        {
            return ingredient_index;
        }
    }
    return -1;
}

void add_ingredient(Restaurant_Info* restaurant, char* ingredient_name, int count)
{
    int ingredient_index = find_ingredient_index(restaurant, ingredient_name);
    if (ingredient_index >= 0)
    {
        restaurant->ingredients[ingredient_index].count += count;
    }
    else
    {
        if (restaurant->ingredients_counter == 0)
            restaurant->ingredients = (ingredient* )malloc((restaurant->ingredients_counter + 1) * sizeof(ingredient));
        else
            restaurant->ingredients = (ingredient* )realloc(restaurant->ingredients, (restaurant->ingredients_counter + 1) * sizeof(ingredient));

        restaurant->ingredients[restaurant->ingredients_counter].name = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
        sprintf(restaurant->ingredients[restaurant->ingredients_counter].name, "%s", ingredient_name);

        restaurant->ingredients[restaurant->ingredients_counter].count = count;
        restaurant->ingredients_counter++;
    }
}

void order_ingredient(Restaurant_Info* restaurant, unsigned short supplier_tcp_port, char* ingredient_name, int ingredient_count)
{
    Msg sending_msg;
    sending_msg.type = Restaurant;
    sending_msg.client_tcp_port = restaurant->tcp_port;
    sending_msg.client_username = restaurant->name;
    char* result;
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "order/ingredient/%s/%d", ingredient_name, ingredient_count);
    result = (char*)realloc(result, strlen(result) + 1);
    sending_msg.detail = result;

    char log_msg[INP_BUFF_SIZE];
    sprintf(log_msg, ORDERED_INGREDIENT_LOG, restaurant->name, ingredient_name, supplier_tcp_port);
    log_write(restaurant->name, log_msg);

    char* sending_msg_str = encode_msg(sending_msg);
    int fd = connect_tcp_client(supplier_tcp_port);
    send_tcp_msg(fd, sending_msg_str, 10);
    write(1,WAITING_FOR_SUPPLIER,strlen(WAITING_FOR_SUPPLIER));

    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    freeze_cmd();
    alarm(RESTAURANT_WAITING_TIME);
    int tcp_fd = accept_tcp_client(restaurant->tcp_fd_server);
    if(tcp_fd >= 0)
    {
        char *msg_str = receive_tcp(tcp_fd);
        Msg msg = decode_msg(msg_str);
        if (!strcmp(msg.detail , ACCEPTS_ORDER))
        {
            char* result;
            result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, "%s accepted and your ingredient is ready!\n", msg.client_username);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
            add_ingredient(restaurant, ingredient_name, ingredient_count);
        }
        else if(!strcmp(msg.detail , REJECTS_ORDER))
        {
            char* result;
            result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, "%s rejected and cry about it retard!\n", msg.client_username);
            result = (char*)realloc(result, strlen(result) + 1);
            write(1,result,strlen(result));
        }
    }
    alarm(0);
    active_cmd();
    if (tcp_fd < 0)
    {
        write(1,TIME_OUT_MSG, strlen(TIME_OUT_MSG));
        char log_msg[INP_BUFF_SIZE];
        sprintf(log_msg, TIMEOUT_INGREDIENT_ORDER_LOG, restaurant->name);
        log_write(restaurant->name, log_msg);
    }
}

void answer_order(Restaurant_Info* restaurant)
{
    write(1, ENTER_CUSTOMER_PORT, strlen(ENTER_CUSTOMER_PORT));
    char port[INP_BUFF_SIZE] = {0};
    int bytes_read = read(STDIN_FILENO, port, INP_BUFF_SIZE);
    port[bytes_read - 1] = '\0';

    write(1, ANSWER_PROPMT, strlen(ANSWER_PROPMT));
    char answer[INP_BUFF_SIZE] = {0};
    bytes_read = read(STDIN_FILENO, answer, INP_BUFF_SIZE);
    answer[bytes_read - 1] = '\0';

    int order_index = find_order_index(restaurant, atoi(port));
    if(order_index != -1)
    {
        adjust_order_status(&restaurant->orders[order_index]);
        if(restaurant->orders[order_index].status == Accepted || restaurant->orders[order_index].status == Rejected)
        {
            write(1, ALREADY_ANSWERED_ORDER, strlen(ALREADY_ANSWERED_ORDER));
            return;
        }
        else if(restaurant->orders[order_index].status == TimeOut)
        {
            write(1, ALREADY_TIMEDOUT_ORDER, strlen(ALREADY_TIMEDOUT_ORDER));
            return;
        }
        else if(restaurant->orders[order_index].status == Wating)
        {
            if(!strcmp(answer,ACCEPTS_ORDER))
            {
                int food_index = find_food_index(restaurant, order_index);
                int have_enough_ingredients_flag = have_enough_ingredients(restaurant, food_index);
                if(!have_enough_ingredients_flag || restaurant->ingredients_counter == 0)
                {
                    not_enough_ingredients(restaurant,atoi(port));
                    restaurant->orders[order_index].status = Rejected;

                    char log_msg[INP_BUFF_SIZE];
                    sprintf(log_msg, REJECT_FOOD_LOG, restaurant->name, restaurant->orders[order_index].name, restaurant->orders[order_index].customer_tcp_port);
                    log_write(restaurant->name, log_msg);
                }
                else
                {
                    decrease_ingredients(restaurant, food_index);
                    send_accept_msg(restaurant, atoi(port));
                    restaurant->orders[order_index].status = Accepted;
                    char log_msg[INP_BUFF_SIZE];
                    sprintf(log_msg, ACCEPTED_FOOD_LOG, restaurant->name, restaurant->orders[order_index].name, restaurant->orders[order_index].customer_tcp_port);
                    log_write(restaurant->name, log_msg);
                }
            }
            if(!strcmp(answer,REJECTS_ORDER))
            {
                reject_order(restaurant, atoi(port));
                restaurant->orders[order_index].status = Rejected;
                char log_msg[INP_BUFF_SIZE];
                sprintf(log_msg, REJECT_FOOD_LOG, restaurant->name, restaurant->orders[order_index].name, restaurant->orders[order_index].customer_tcp_port);
                log_write(restaurant->name, log_msg);
            }
        }
    }
}

void restaurant_command_handler(char* command, Restaurant_Info* restaurant)
{
    if(!strcmp(command, START_WORKING))
    {
        start_working(restaurant);
    }
    else if(!strcmp(command, STOP_WORKING))
    {
        stop_working(restaurant);
    }
    else if(!strcmp(command, SHOW_RECIPES_PROMPT))
    {
        show_recipes(restaurant->recipes);
    }
    else if(!strcmp(command, SHOW_INGREDIENTS_PROMPT))
    {
        show_ingredients(restaurant);
    }
    else if(!strcmp(command, SHOW_SUPPLIERS_PROMPT))
    {
        show_suppliers(restaurant);
    }
    else if(!strcmp(command, SHOW_ORDERS_PROMPT))
    {
        show_orders(restaurant);
    }
    else if(!strcmp(command, SHOW_HISTORY_PROMPT))
    {
        show_sales_history(restaurant);
    }
    else if(!strcmp(command, ANSWER_ORDER_PROMPT))
    {
        answer_order(restaurant);
    }
    else if(!strcmp(command, ORDER_INGREDIENT_PROMPT))
    {
        write(1, ENTER_SUPPLIER_PORT, strlen(ENTER_SUPPLIER_PORT));
        char port[INP_BUFF_SIZE] = {0};
        int bytes_read = read(STDIN_FILENO, port, INP_BUFF_SIZE);
        port[bytes_read - 1] = '\0';

        write(1, ENTER_INGREDIENT_NAME, strlen(ENTER_INGREDIENT_NAME));
        char ingredient_name[INP_BUFF_SIZE] = {0};
        bytes_read = read(STDIN_FILENO, ingredient_name, INP_BUFF_SIZE);
        ingredient_name[bytes_read - 1] = '\0';

        write(1, ENTER_INGREDIENT_COUNT, strlen(ENTER_INGREDIENT_COUNT));
        char count[INP_BUFF_SIZE] = {0};
        bytes_read = read(STDIN_FILENO, count, INP_BUFF_SIZE);
        count[bytes_read - 1] = '\0';

        order_ingredient(restaurant, atoi(port), ingredient_name, atoi(count));
    }
}

void restaurant_tcp_handler(Msg msg, Restaurant_Info *restaurant)
{
    if(!strcmp(msg.detail , SUPPLIER_INTRUDUCE_ITSELF))
    {
        print_supplier(&msg);
    }
    else
    {
        ParsedLine parsed_detail = custom_strtok(msg.detail,"/");
        if(!strcmp(parsed_detail.argv[0], "order") && !strcmp(parsed_detail.argv[1], "food"))
        {
            get_order(restaurant, &parsed_detail, msg.client_tcp_port);
            char log_msg[INP_BUFF_SIZE];
            sprintf(log_msg, RECIEVE_ORDER, restaurant->name, msg.client_username, msg.client_tcp_port);
            log_write(restaurant->name, log_msg);
        }
    }
}

void run(Restaurant_Info* restaurant, struct sockaddr_in* addrOut)
{
    fd_set master_set, working_set;
    int max_sd = restaurant->tcp_fd_server;
    FD_ZERO(&master_set);
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(restaurant->udp_fd, &master_set);
    FD_SET(restaurant->tcp_fd_server, &master_set);

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
                if (i == restaurant->udp_fd)
                {
                    char *msg_str = rcv_udp(restaurant->udp_fd);
                    Msg msg = decode_msg(msg_str);
                    udp_handler(&msg, restaurant->name, restaurant->tcp_port, Restaurant);
                    
                }
                else if(i == restaurant->tcp_fd_server)
                {
                    int new_socket = accept_tcp_client(restaurant->tcp_fd_server);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }
                else if (i == STDIN_FILENO)
                {
                    int bytes_read = read(STDIN_FILENO, buffer, INP_BUFF_SIZE);
                    buffer[bytes_read - 1] = '\0';
                    restaurant_command_handler(buffer, restaurant);
                }
                else
                {
                    char *msg_str = receive_tcp(i);
                    Msg msg = decode_msg(msg_str);
                    restaurant_tcp_handler(msg, restaurant);
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr_out;
    Restaurant_Info restaurant;
    restaurant_signin(&restaurant, &addr_out, (unsigned short)atoi(argv[1]));
    run(&restaurant, &addr_out);
}