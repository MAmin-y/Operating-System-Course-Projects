#ifndef __BILL_CALCULATOR_H__
#define __BILL_CALCULATOR_H__

#include "Named_pipe.hpp"
#include "Utils.hpp"
#include <memory>
#include "log.hpp"
#include "Consts.hpp"

class Bill_Calculator
{
private:
    int buildings_number;
    string client_name;
    shared_ptr<Named_Pipe_Client> client_bill_pipe;
    shared_ptr<Named_Pipe_Server> server_bill_pipe;
    vector<MonthReport> electricity;
    vector<MonthReport> water;
    vector<MonthReport> gas;

public:
    Bill_Calculator(int buildings_number_);
    ~Bill_Calculator();
    void collect_month_records(string report_str);
    void calc_bill();
    void calc_electricity_bills();
    void calc_water_bills();
    void calc_gas_bills();
    void listen();
};

#endif