#ifndef __MAIN_H__
#define __MAIN_H__

#include "Ordinary_pipe.hpp"
#include "Utils.hpp"
#include "log.hpp"

class Main
{
private:
    std::vector<std::string> buildings_requested;
    std::string resources_requested;
    std::string report_requested;
    map<std::string, int> building_names_map;
    std::vector<std::vector<int>> read_pipes;
    std::vector<std::vector<int>> write_pipes;
    string buildings_dir_path;
public:
    Main(string buildings_dir_path_);
    ~Main();
    void get_user_request();
    bool is_building_requested(string building_name);
    void send_user_request_to_buildings();
    void make_buildings();
    void make_bill_calculato();
    void write_output();
    void wait_for_all_children();
    void log_all_buildings_names();
};

#endif