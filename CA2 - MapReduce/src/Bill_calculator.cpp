#include "Bill_calculator.hpp"

using namespace std;

Bill_Calculator::Bill_Calculator(int buildings_number_)
{
    buildings_number = buildings_number_;
    const string BILL_SERVER_NAME_str = BILL_SERVER_NAME;
    server_bill_pipe = make_shared<Named_Pipe_Server>(BILL_SERVER_NAME_str);
    client_bill_pipe = nullptr;
    log_info(BILL_CALCULATOR_LOG_NAME, "bill calculator created");
}

void Bill_Calculator::calc_bill()
{
    calc_electricity_bills();
    calc_water_bills();
    calc_gas_bills();
}

Bill_Calculator::~Bill_Calculator()
{
    string BILL_SERVER_NAME_str = BILL_SERVER_NAME;
    Named_Pipe::remove_pipe(BILL_SERVER_NAME_str);
}

void Bill_Calculator::collect_month_records(string report_str)
{
    int i = 0;
    vector<string> report = tokenize(report_str, "&");
    client_name = report[0];
    client_bill_pipe = make_shared<Named_Pipe_Client>(client_name);
    log_info(BILL_CALCULATOR_LOG_NAME, "bill calculator received data from building " + client_name);
    vector<string> resource_reports = tokenize(report[1], "#");
    while (i < int(resource_reports.size()))
    {
        vector<string> resource = tokenize(resource_reports[i], "@");
        if (ELECTRICITY_RESOURCE == resource[0])
        {
            electricity = month_report_decoder(resource[1]);
        }
        else if (WATER_RESOURCE == resource[0])
        {
            water = month_report_decoder(resource[1]);
        }
        else if (GAS_RESOURCE == resource[0])
        {
            gas = month_report_decoder(resource[1]);
        }
        i++;
    }
}

void Bill_Calculator::calc_electricity_bills()
{
    for (int i=0; i<int(electricity.size()); i++)
    {
        electricity[i].bill = (electricity[i].total_usage - (electricity[i].total_usage_in_peak + electricity[i].total_usage_in_lowest_hour)) +
                            (electricity[i].total_usage_in_peak * 1.25) + (electricity[i].total_usage_in_lowest_hour * 0.75);
    }
}

void Bill_Calculator::calc_water_bills()
{
    for (int i=0; i<int(water.size()); i++)
    {
        water[i].bill = (water[i].total_usage - water[i].total_usage_in_peak) + (water[i].total_usage_in_peak * 1.25);
    }
}

void Bill_Calculator::calc_gas_bills()
{
    for (int i=0; i<int(gas.size()); i++)
    {
        gas[i].bill = gas[i].total_usage;
    }
}

void Bill_Calculator::listen()
{
    string report;
    int read_buildings_counter = 0;
    while (read_buildings_counter < buildings_number)
    {
        report = server_bill_pipe->receive();
        if (report != "")
        {
            collect_month_records(report);
            calc_bill();
            string sending_msg = encode_report_for_main(electricity, water, gas, BILL_REPORT_TYPE);
            client_bill_pipe->send(sending_msg);
            log_info(BILL_CALCULATOR_LOG_NAME, "bill calculator sent data to building " + client_name);
            read_buildings_counter++;
        }
    }
}
