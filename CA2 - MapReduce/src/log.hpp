#ifndef __LOG_H__
#define __LOG_H__

#include <string>

void log_info(const std::string& name, const std::string& msg);
void log_error(const std::string& name, const std::string& msg);

#endif