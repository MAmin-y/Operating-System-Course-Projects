#include "Ordinary_pipe.hpp"
using namespace std;

vector<int> create_pipe()
{
    vector<int> result;
    int fd[2];
    if (pipe(fd) == -1)
    {
        log_error(ORDINARY_PIPE_LOG_NAME ,"Failed to create pipe");

    }
    result.push_back(fd[0]);
    result.push_back(fd[1]);
    return result;
}

bool close_fd(int fd)
{
    if (close(fd) == -1)
    {
        log_error(ORDINARY_PIPE_LOG_NAME ,"Failed to close the pipe");
        return false;
    }

    return true;
}

int write_ordinary_pipe(const char *buffer, int size, int write_fd)
{
    int bytes_written = write(write_fd, buffer, size);
    if (bytes_written == -1)
    {
        log_error(ORDINARY_PIPE_LOG_NAME ,"Failed to write to pipe");

    }

    return bytes_written;
}

string read_fd(int read_fd)
{
    char buffer[BUFF_SIZE];
    string result;

    while (true)
    {
        int read_bytes = read(read_fd, buffer, BUFF_SIZE);

        if (read_bytes == -1)
        {
            if (errno == EAGAIN)
            {
                break;
            }
            else
            {
                log_error(ORDINARY_PIPE_LOG_NAME ,"Failed to read from pipe");
            }
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