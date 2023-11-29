#include "log.hpp"
#include <iostream>
#include <string>
#include "Consts.hpp"

using namespace std;

void log_info(const string& name, const string& msg)
{
    string total_msg = PINK_COLOR + "LOG " + "INFO --" + name +  ": " + msg + RESET_COLOR;
    cout << total_msg << endl;
}

void log_error(const string& name, const string& msg)
{
    string total_msg = RED_COLOR + "LOG " + "ERROR --" + name +  ": " + msg + RESET_COLOR;
    cerr << total_msg << endl;
}