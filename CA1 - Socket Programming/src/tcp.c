#include "tcp.h"

int connect_tcp_server(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1 ) {
        close(server_fd);
        return -1;
    }
    listen(server_fd, 100);
    return server_fd;
}

int accept_tcp_client(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&address_len);
    return client_fd;
}

int connect_tcp_client(int port) {
    int fd;
    struct sockaddr_in server_address;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST_ADDR);
    if (connect(fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }
    return fd;
}

int send_tcp_msg(int sock_fd, const char* msg, int max_tries) {
    int left_bytes = strlen(msg);
    int tries = 0;
    for (; tries <= max_tries && left_bytes > 0; ++tries) {
        int sent_bytes = send(sock_fd, msg, left_bytes, 0);
        left_bytes -= sent_bytes;
    }
    return left_bytes;
}

char* receive_tcp(int sock_fd) {
    int num_bytes = 0, cur_size = 0, total_size = 0;
    char buf[1024] = {0};
    char* result = NULL;
    for (;;) {
        num_bytes = recv(sock_fd, buf, 1024, 0);
        if (num_bytes <= 0)
            return result;
        total_size += num_bytes;
        result = (char*)realloc(result, cur_size + num_bytes);
        memcpy(&result[cur_size], buf, num_bytes);
        cur_size += num_bytes;
        if (num_bytes < 1024)
            break;
        num_bytes = 0;
    }
    result = (char*)realloc(result, total_size + 1);
    result[total_size] = '\0';
    return result;
}