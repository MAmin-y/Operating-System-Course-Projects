#ifndef __NAMED_PIPE_H__
#define __NAMED_PIPE_H__

#include <string>
#include <iostream>
#include "Consts.hpp"
#include "log.hpp"
using namespace std;

class Named_Pipe
{
public:
    Named_Pipe(const string &pipe_name);

    static void remove_pipe(const string &pipe_path);

protected:
    string pipe_name_;
    int pipe_fd_;
};

class Named_Pipe_Client : Named_Pipe
{
public:
    Named_Pipe_Client(const string &pipe_name);

    void send(const string &msg);

    ~Named_Pipe_Client();
};

class Named_Pipe_Server : Named_Pipe
{
public:
    Named_Pipe_Server(const string &pipe_name);

    string receive();

    ~Named_Pipe_Server();
};

#endif
