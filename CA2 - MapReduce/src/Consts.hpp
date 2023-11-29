#ifndef __CONSTS_H__
#define __CONSTS_H__

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <sstream>

using namespace std;

const int BUFF_SIZE = 10000;

const std::string PATH_TO_BULDING_PROGRAM = "./Building.out";
const std::string PATH_TO_RESOURCE_PROGRAM = "./Resource_calculator.out";
const std::string PATH_TO_BILL_PROGRAM = "./Bill_calculator.out";

const int RESOURCES_NUMBER = 3;
const int MONTH_NUMBER = 12;
const int DAY_NUMBER = 30;
const int HOUR_NUMBER = 6;

const std::string WATER_RESOURCE = "Water";
const std::string GAS_RESOURCE = "Gas";
const std::string ELECTRICITY_RESOURCE = "Electricity";

const std::string BILL_SERVER_NAME = "Bill_Calculator";

const std::string GET_BUILDINGS_PROMPT = ">>>>> Please write buildings you need the report for?";
const std::string GET_RESOURCES_PROMPT = ">>>>> which resources you need the report for ? your options are: Electricity / Water / Gas";
const std::string GET_REPORT_PROMPT = ">>>>> which field you need ? choose one of: Total-Usage / Average-Usage / Peak-Hour / Bill / Difference-between-peak-and-average";

const std::string EMPTY_RESOURCE = "EMPTY";

const std::string SPACE_DELIM = " ";
const std::string FORWARD_SLASH_DELIM = "/";
const std::string COLON_DELIM = ":";
const std::string PERCENT_DELIM = "%";
const std::string COMMA_DELIM = ",";

const std::string TOTAL_USAGE_REPORT_TYPE = "Total-Usage";
const std::string AVERAGE_USAGE_REPORT_TYPE = "Average-Usage";
const std::string PEAK_HOURS_REPORT_TYPE = "Peak-Hour";
const std::string BILL_REPORT_TYPE = "Bill";
const std::string DIFF_PEAK_AVERAGE_REPORT_TYPE = "Difference-between-peak-and-average";

const std::string RESET_COLOR = "\033[0m";
const std::string RED_COLOR = "\033[31m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string LIGHT_BLUE_COLOR = "\033[36m";
const std::string GREEN_COLOR = "\033[0;32m";
const std::string DARK_BLUE_COLOR = "\033[0;34m";
const std::string PINK_COLOR = "\033[1;35m";

const std::string RESOURCE_CALCULATOR_LOG_NAME = "Resource Calculator";
const std::string MAIN_LOG_NAME = "Main";
const std::string BILL_CALCULATOR_LOG_NAME = "Bill Calculator";
const std::string BUILDING_LOG_NAME = "Building";
const std::string NAMED_PIPE_LOG_NAME = "Named Pipe";
const std::string ORDINARY_PIPE_LOG_NAME = "Ordinary Pipe";

enum ResourceType
{
    Electricity,
    Water,
    Gas
};

struct MonthReport
{
    int year_number;
    int month_number;
    int total_usage;
    int average_usage;
    int peak_hour;
    int total_usage_in_peak;
    int diff_peak_average;
    int total_usage_in_lowest_hour;
    int bill;
};

#endif