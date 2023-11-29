#include "utils.h"

int generate_tcp_prot()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Socket binding failed");
        close(sockfd);
        return -1;
    }

    socklen_t addrLen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addrLen) < 0)
    {
        perror("getsockname failed");
        close(sockfd);
        return -1;
    }

    int port = ntohs(addr.sin_port);
    close(sockfd);

    return port;
}

void alarm_handler(int sig)
{
    return;
}

char* get_username(unsigned short udp_port, unsigned short tcp_port, Client type, int tcp_fd_server)
{
    Msg msg_struct;
    char buffer[INP_BUFF_SIZE];
    msg_struct.client_tcp_port = tcp_port;
    msg_struct.detail = CHECK_USERNAME_COMMAND;
    msg_struct.type = type;
    write(STDIN_FILENO, SIGNIN_PROMPT, strlen(SIGNIN_PROMPT));
    while(1)
    {
        int bytes_read = read(STDIN_FILENO, buffer, INP_BUFF_SIZE);
        buffer[bytes_read-1] = '\0';
        msg_struct.client_username = (char*)malloc(bytes_read);
        strcpy(msg_struct.client_username, buffer);
        char* msg= encode_msg(msg_struct);
        broadcast_msg(udp_port, msg);
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        alarm(2);
        int fd = accept_tcp_client(tcp_fd_server);
        alarm(0);        
        if (fd < 0)
            break;
        write(STDIN_FILENO, INVALID_SIGNIN_PROMPT, strlen(INVALID_SIGNIN_PROMPT));
    }
    return msg_struct.client_username;
}

char *read_line(int fd, int boundary)
{
    char *result = NULL;
    int curr_size = 0;
    for (; boundary != 0; --boundary)
    {
        ++curr_size;
        result = (char *)realloc(result, curr_size);
        read(fd, &result[curr_size - 1], 1);
        if (result[curr_size - 1] == '\n')
        {
            result[curr_size - 1] = '\0';
            break;
        }
    }
    return result;
}

ParsedLine custom_strtok(char *input_line, const char *delimiter)
{
    ParsedLine result = {0, NULL};
    int input_line_len = strlen(input_line);
    int last_tok_end = 0;
    for (int i = 0; i <= input_line_len; ++i)
    {
        if (strchr(delimiter, input_line[i]) != NULL)
        {
            result.argv = (char **)realloc(result.argv, result.count + 1);
            result.argv[result.count] = (char *)calloc(i - last_tok_end + 1, sizeof(char));
            memcpy(result.argv[result.count], &input_line[last_tok_end], i - last_tok_end);
            result.argv[result.count][i - last_tok_end] = '\0';
            ++result.count;
            last_tok_end = i + 1;
        }
    }
    return result;
}

char* encode_msg(Msg msg)
{
    char* result;
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "%s-%d-%d-%s", msg.client_username, msg.type, msg.client_tcp_port, msg.detail);
    result = (char*)realloc(result, strlen(result) + 1);
    return result;
}

Msg decode_msg(char *msg)
{
    char **argv = custom_strtok(msg, "-").argv;
    Msg msg_struct;
    msg_struct.client_username = argv[0];
    msg_struct.type = (Client)atoi(argv[1]);
    msg_struct.client_tcp_port = (unsigned short)atoi(argv[2]);
    msg_struct.detail = argv[3];
    return msg_struct;
}

void udp_handler(Msg* msg, char* username, unsigned short host_tcp_port, Client type)
{
    if(msg->client_tcp_port != host_tcp_port)
    {    
        if(!strcmp(msg->detail, CHECK_USERNAME_COMMAND) && !strcmp(msg->client_username, username))
        {
            int fd = connect_tcp_client(msg->client_tcp_port);
        }
        else if(!strcmp(msg->detail, START_WORKING) && type == Customer)
        {
            char* result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, OPENED_RESTAURANT_MSG, msg->client_username);
            write(1,result, strlen(result));
        }
        else if(!strcmp(msg->detail, STOP_WORKING) && type == Customer)
        {
            char* result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
            sprintf(result, CLOSED_RESTAURANT_MSG, msg->client_username);
            write(1,result, strlen(result));
        }
        else if(!strcmp(msg->detail, SHOW_RESTAURANT_MSG_DETAIL) && type == Restaurant)
        {
            Msg sending_msg;
            sending_msg.type = type;
            sending_msg.client_tcp_port = host_tcp_port;
            sending_msg.client_username = username;
            sending_msg.detail = RESTAURANT_INTRUDUCE_ITSELF;
            char* sending_msg_str = encode_msg(sending_msg);
            int fd = connect_tcp_client(msg->client_tcp_port);
            send_tcp_msg(fd, sending_msg_str, 10);
        }
        else if(!strcmp(msg->detail, SHOW_SUPPLIER_MSG_DETAIL) && type == Supplier)
        {
            Msg sending_msg;
            sending_msg.type = type;
            sending_msg.client_tcp_port = host_tcp_port;
            sending_msg.client_username = username;
            sending_msg.detail = SUPPLIER_INTRUDUCE_ITSELF;
            char* sending_msg_str = encode_msg(sending_msg);
            int fd = connect_tcp_client(msg->client_tcp_port);
            send_tcp_msg(fd, sending_msg_str, 10);
        }
    }
    
}

char* create_welcome_prompt(char* username, Client type)
{
    char* type_str;
    char* result;
    if(type == Restaurant) type_str = "Restaurant";
    else if(type == Customer) type_str = "Customer";
    else if(type == Supplier) type_str = "Supplier";
    result = (char*)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "Wellcome %s as %s!\n", username, type_str);
    result = (char*)realloc(result, strlen(result) + 1);
    return result;
}

void freeze_cmd()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void active_cmd()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}