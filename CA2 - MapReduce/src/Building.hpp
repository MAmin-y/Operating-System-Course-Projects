#ifndef __BULDING_H__
#define __BULDING_H__

#include "Ordinary_pipe.hpp"
#include "Utils.hpp"
#include "Named_pipe.hpp"
#include "log.hpp"
#include <memory>

class Building
{
private:
    int write_pipe_fd;
    int read_pipe_fd;
    string path;
    vector<vector<int>> resources_read_pipes;
    vector<MonthReport> electricity;
    vector<MonthReport> water;
    vector<MonthReport> gas;
    shared_ptr<Named_Pipe_Client> client_building_pipe;
    shared_ptr<Named_Pipe_Server> server_bill_pipe;
    string all_resource_reprts;
    string report_requested;
    vector<string> resource_requested;

public:
    Building(string path_, int write_pipe_fd_, int read_pipe_fd_);
    ~Building();
    void get_main_requests();
    void make_resource_processes();
    void collect_resource_reports();
    string encode_report(string report_requested);
    void send_report_to_bill_calculator();
    string recieve_bills();
    bool is_resource_requested(string resource_name);
    void wait_for_all_children();
    void write_result_for_main();
};

#endif