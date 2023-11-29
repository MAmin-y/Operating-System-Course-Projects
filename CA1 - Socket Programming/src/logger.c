#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include "logger.h"
#include "consts.h"

void create_file(char* app_name)
{
    char file_name[INP_BUFF_SIZE];
    sprintf(file_name, "%s.log", app_name);
    int fd = creat( file_name, READ_WRITE_FILE_FORMAT );
    if(fd < 0)
    {
        write(1, "error making file!\n", strlen("error making file!\n"));
        return;
    }
        
    close(fd);
}

void log_write(char* app_name, char* msg)
{
    char file_name[INP_BUFF_SIZE];
    sprintf(file_name, "%s.log", app_name);
    int fd = open(file_name, O_WRONLY | O_APPEND);
    char buf[INP_BUFF_SIZE];
    sprintf(buf, "%s\n", msg);
    write(fd, buf, strlen(buf));
    close(fd);
}