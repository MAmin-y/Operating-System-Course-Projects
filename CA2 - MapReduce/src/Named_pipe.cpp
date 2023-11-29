#include "Named_pipe.hpp"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>
#include <string>

Named_Pipe::Named_Pipe(const std::string& pipe_name)
    : pipe_name_(pipe_name) {}

void Named_Pipe::remove_pipe(const std::string& pipe_path) {
    remove(pipe_path.c_str());
}

Named_Pipe_Client::Named_Pipe_Client(const std::string& pipe_name)
    : Named_Pipe(pipe_name) {
    do {
        pipe_fd_ = open(pipe_name.c_str(), O_WRONLY);
    } while (pipe_fd_ == -1);
}

void Named_Pipe_Client::send(const std::string& msg) {
    int sent_bytes = write(pipe_fd_, msg.c_str(), msg.size());
    if (sent_bytes != int(msg.size()))
        log_error(NAMED_PIPE_LOG_NAME ,"Couldn't send complete msg over pipe.");
}

Named_Pipe_Server::Named_Pipe_Server(const std::string& pipe_name)
    : Named_Pipe(pipe_name) {
    if ((pipe_fd_ = open(pipe_name.c_str(), O_RDONLY | O_NONBLOCK)) == -1) {
        if (mkfifo(pipe_name.c_str(), 0777) != 0) {
            log_error(NAMED_PIPE_LOG_NAME ,"Couldn't make FIFO file.");

        }
        else
            pipe_fd_ = open(pipe_name_.c_str(), O_RDONLY | O_NONBLOCK);
    }
    if (pipe_fd_ == -1)
        log_error(NAMED_PIPE_LOG_NAME ,"Couldn't open server side of named pipe.(" + pipe_name + ")");

}

Named_Pipe_Client::~Named_Pipe_Client() {
    close(pipe_fd_);
}

std::string Named_Pipe_Server::receive() {
    char buffer[BUFF_SIZE];
    string result;

    while (true)
    {
        int read_bytes = read(pipe_fd_, buffer, BUFF_SIZE);

        if (read_bytes == -1)
        {
            break;
        }
        else if (read_bytes < BUFF_SIZE)
        {
            result.append(buffer, read_bytes);
            break;
        }

        result.append(buffer, read_bytes);
    }
    return result;
}

Named_Pipe_Server::~Named_Pipe_Server()
{
    close(pipe_fd_);
}