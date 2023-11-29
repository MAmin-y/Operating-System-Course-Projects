#ifndef __RESOURCE_HPP
#define __RESOURCE_HPP

#include "Ordinary_pipe.hpp"
#include "Utils.hpp"
#include "log.hpp"

class Resource
{
private:
    int write_pipe_fd;
    std::string resource_file_path;
    ResourceType res_type;
    std::vector<MonthReport> resource_reports;

public:
    void read_csv_file();
    void calc_report_field(std::vector<std::vector<int>> &month_usages_matrix, MonthReport &report);
    int calc_total_usage(std::vector<std::vector<int>> &month_usages_matrix);
    int calc_average_usage(int total_usage);
    int calc_peak_hour(std::vector<std::vector<int>> &month_usages_matrix);
    int calc_total_usage_in_peak(std::vector<std::vector<int>> &month_usages_matrix, int peak);
    int calc_difference_from_average_in_peak(int whole_usage, int peak_usage);
    int calc_total_usage_in_min_hours(vector<vector<int>> &month_usages_matrix);
    std::vector<MonthReport> get_resource_reports();
    int get_write_pipe_fd();
    Resource(string file_path_, int pipe_fd_, ResourceType res_type_);
    ~Resource();
    void send_data_to_building(string encoded_records);
    void parse_one_line(string line, bool& is_month_finished, MonthReport& report, 
                                        vector<vector<int>>& month_usages_matrix);

};

#endif