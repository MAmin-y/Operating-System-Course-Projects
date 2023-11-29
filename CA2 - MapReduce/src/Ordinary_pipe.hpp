#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <sstream>
#include "Consts.hpp"
#include "log.hpp"

std::vector<int> create_pipe();

bool close_fd(int fd);

int write_ordinary_pipe(const char *buffer, int size, int write_fd);

std::string read_fd(int read_fd);