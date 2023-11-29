#include "main.hpp"

using namespace std;

Main::Main(string buildings_dir_path_)
{
    buildings_dir_path = buildings_dir_path_;
}

Main::~Main()
{
    for (int i = 0; i < int(buildings_requested.size()); i++)
    {
        close_fd(write_pipes[i][1]);
    }
}

void Main::get_user_request()
{
    string buidings_requested_str;
    cout << LIGHT_BLUE_COLOR << GET_BUILDINGS_PROMPT << RESET_COLOR << endl;
    getline(cin, buidings_requested_str);
    cout << LIGHT_BLUE_COLOR << GET_RESOURCES_PROMPT << RESET_COLOR << endl;
    getline(cin, resources_requested);
    cout << LIGHT_BLUE_COLOR << GET_REPORT_PROMPT << RESET_COLOR << endl;
    cin >> report_requested;
    buildings_requested = tokenize(buidings_requested_str, SPACE_DELIM);
}

bool Main::is_building_requested(string building_name)
{
    for (int i=0; i<int(buildings_requested.size()); i++)
    {
        if(building_name == buildings_requested[i])
            return true;
    }
    return false;
}

void Main::send_user_request_to_buildings()
{
    map<string, int>::iterator it = building_names_map.begin();
    while (it != building_names_map.end())
    {
        string building;
        for (int i = 0; i<int(buildings_requested.size()); i++)
        {
            if (it->first == buildings_requested[i])
            {
                string msg = resources_requested + FORWARD_SLASH_DELIM + report_requested;
                write_ordinary_pipe(msg.c_str(), msg.size(), write_pipes[it->second][1]);
            }
        }
        ++it;
    }
    log_info(MAIN_LOG_NAME, "users' requests are sent to buildings");
}

void Main::make_buildings()
{
    int folder_count = 0;
    for (const auto &entry : filesystem::directory_iterator(buildings_dir_path))
    {
        if (entry.is_directory())
        {
            string building_name = entry.path().filename().string();
            if(!is_building_requested(building_name))
                continue;
            read_pipes.push_back(create_pipe());
            write_pipes.push_back(create_pipe());
            building_names_map[building_name] = folder_count;
            int pid = fork();
            if (pid == -1)
            {
                log_error(MAIN_LOG_NAME, "failed to creat building");
            }
            else if (!pid)
            {
                close_fd(read_pipes[folder_count][0]);
                close_fd(write_pipes[folder_count][1]);

                execl(PATH_TO_BULDING_PROGRAM.c_str(), PATH_TO_BULDING_PROGRAM.c_str(), 
                        (buildings_dir_path + FORWARD_SLASH_DELIM + building_name).c_str(),
                            to_string(read_pipes[folder_count][1]).c_str(), 
                            to_string(write_pipes[folder_count][0]).c_str(),nullptr);
                log_error(MAIN_LOG_NAME, "failed to execute building");
                return;
            }
            else
            {
                close_fd(read_pipes[folder_count][1]);
                close_fd(write_pipes[folder_count][0]);
            }
            ++folder_count;
        }
    }
}

void Main::make_bill_calculato()
{
    int pid = fork();

    if (pid == -1)
    {
        log_error(MAIN_LOG_NAME, "failed to creat bill calculator");
    }
    else if (!pid)
    {
        execl(PATH_TO_BILL_PROGRAM.c_str(), PATH_TO_BILL_PROGRAM.c_str(), to_string(buildings_requested.size()).c_str(), nullptr);
        log_error(MAIN_LOG_NAME, "failed to execute bill calculator");
        return;
    }
}

void Main::write_output()
{
    map<string, int>::iterator it = building_names_map.begin();
    while (it != building_names_map.end())
    {
        cout << GREEN_COLOR <<"\n******* Building: " << it->first << " *******" << RESET_COLOR << endl;
        string data_str = read_fd(read_pipes[it->second][0]);
        vector<string> data = tokenize(data_str, PERCENT_DELIM);
        for (int i=0; i < 6; i+=2)
        {
            if(data[i+1] != EMPTY_RESOURCE)
            {
                cout << YELLOW_COLOR << data[i] << COLON_DELIM << RESET_COLOR << endl;
                vector<string> one_resource_all_months = tokenize(data[i+1], FORWARD_SLASH_DELIM);
                for (int j=0; j<int(one_resource_all_months.size()); j++)
                    cout << one_resource_all_months[j];
            }
        }
        close(read_pipes[it->second][0]);
        ++it;
    }
}

void Main::wait_for_all_children()
{
    for (int i=0; i<=int(buildings_requested.size()); i++)
    {
        wait(NULL);
    }
}

void Main::log_all_buildings_names()
{

    int folder_count = 0;
    for (const auto &entry : filesystem::directory_iterator(buildings_dir_path))
    {
        if (entry.is_directory())
        {
            folder_count++;
            string building_name = entry.path().filename().string();
            log_info(MAIN_LOG_NAME, building_name);
        }
    }
    log_info(MAIN_LOG_NAME, "number of buildings:" + to_string(folder_count));
}