#include "Building.hpp"

Building::Building(string path_, int write_pipe_fd_, int read_pipe_fd_)
{
    path = path_;
    write_pipe_fd = write_pipe_fd_;
    read_pipe_fd = read_pipe_fd_;
    server_bill_pipe = make_shared<Named_Pipe_Server>(tokenize(path_, FORWARD_SLASH_DELIM)[1]);
    client_building_pipe = make_shared<Named_Pipe_Client>(BILL_SERVER_NAME);
    log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], "process for this building created");
}

void Building::get_main_requests()
{
    string resource_report_str;
    while(true)
    {
        resource_report_str = read_fd(read_pipe_fd);
        if (resource_report_str != "")
        {
            break;;
        }
    }
    vector<string> resource_report = tokenize(resource_report_str, FORWARD_SLASH_DELIM);
    report_requested = resource_report[1];
    resource_requested = tokenize(resource_report[0], SPACE_DELIM);
    log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], "user requests are recieved by building");
}

void Building::make_resource_processes()
{
    vector<string> resource_types = {ELECTRICITY_RESOURCE, WATER_RESOURCE, GAS_RESOURCE};
    
    for (int i = 0; i < RESOURCES_NUMBER; i++)
    {
        if (!is_resource_requested(resource_types[i]))
            continue;
        resources_read_pipes.push_back(create_pipe());
        int pid = fork();
        if (pid == -1)
        {
            log_error(tokenize(path, FORWARD_SLASH_DELIM)[1], "failed to creat resource calculator");
        }
        else if (!pid)
        {
            close_fd(resources_read_pipes[resources_read_pipes.size() - 1][0]);
            execl(PATH_TO_RESOURCE_PROGRAM.c_str(), PATH_TO_RESOURCE_PROGRAM.c_str(), 
                (path + FORWARD_SLASH_DELIM + resource_types[i] + ".csv").c_str(),
                    to_string(resources_read_pipes[resources_read_pipes.size() - 1][1]).c_str(),
                        to_string(int(ResourceType(i))).c_str(), resource_types[i].c_str(), nullptr);
                    
            log_error(tokenize(path, FORWARD_SLASH_DELIM)[1], "failed to execute resource calculator");
            return;
        }
        else
            close_fd(resources_read_pipes[resources_read_pipes.size() - 1][1]);
    }
}

void Building::collect_resource_reports()
{
    int i = 0;
    while (i < int(resources_read_pipes.size()))
    {
        string report_str = read_fd(resources_read_pipes[i][0]);
        vector<string> report = tokenize(report_str, "@");
        log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], report[0] + " report is recieved by building");
        if (ELECTRICITY_RESOURCE == report[0])
        {
            electricity = month_report_decoder(report[1]);
            all_resource_reprts += report_str + "#";
        }
        else if (WATER_RESOURCE == report[0])
        {
            water = month_report_decoder(report[1]);
            all_resource_reprts += report_str + "#";
        }
        else if (GAS_RESOURCE == report[0])
        {
            gas = month_report_decoder(report[1]);
            all_resource_reprts += report_str + "#";
        }
        i++;
    }
    all_resource_reprts.pop_back();
}

Building::~Building()
{
    close(write_pipe_fd);
    close(read_pipe_fd);
    for (int i = 0; i < int(resources_read_pipes.size()); i++)
    {
        close(resources_read_pipes[i][0]);
    }
    Named_Pipe::remove_pipe(tokenize(path, FORWARD_SLASH_DELIM)[1]);
}

void Building::send_report_to_bill_calculator()
{
    client_building_pipe->send(tokenize(path, FORWARD_SLASH_DELIM)[1] + "&" + all_resource_reprts);
    log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], "building has sent data to bill calculator");
}

string Building::recieve_bills()
{
    while(true)
    {
        string bill_report = server_bill_pipe->receive();
        if (bill_report != "")
        {
            log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], "building has recieved bills from bill calculator");
            return bill_report;
        }
    }
}

string Building::encode_report(string report_requested)
{
    return encode_report_for_main(electricity, water, gas, report_requested);
}

bool Building::is_resource_requested(string resource_name)
{
    for (int i=0; i<int(resource_requested.size()); i++)
    {
        if(resource_name == resource_requested[i])
            return true;
    }
    return false;
}

void Building::wait_for_all_children()
{
    for (int i=0; i<int(resource_requested.size()); i++)
    {
        wait(NULL);
    }
}

void Building::write_result_for_main()
{
    string building_record_withot_bill;
    string building_record_with_bill;
    string building_record;
    building_record_withot_bill = encode_report(report_requested);
    building_record_with_bill = recieve_bills();

    if (report_requested != BILL_REPORT_TYPE)
        building_record = building_record_withot_bill;
    else
        building_record = building_record_with_bill;
        
    write_ordinary_pipe(building_record.c_str(), building_record.size(), write_pipe_fd);
    log_info(tokenize(path, FORWARD_SLASH_DELIM)[1], "building has sent data to main process");
}