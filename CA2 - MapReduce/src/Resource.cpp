#include "Resource.hpp"

Resource::Resource(string resource_file_path_, int pipe_fd_, ResourceType res_type_)
{
    resource_file_path = resource_file_path_;
    write_pipe_fd = pipe_fd_;
    res_type = res_type_;
    log_info(RESOURCE_CALCULATOR_LOG_NAME, "resource calculator created with addres " + resource_file_path);
}

Resource::~Resource()
{
    close_fd(write_pipe_fd);
}

void Resource::parse_one_line(string line, bool& is_month_finished, MonthReport& report, vector<vector<int>>& month_usages_matrix)
{
    stringstream str(line);
    int day;
    vector<string> parsed_line = tokenize(line, COMMA_DELIM);
    for (int i=0; i<int(parsed_line.size()) ; i++)
    {
        if (i == 0)
            report.year_number = stoi(parsed_line[i]);
        else if (i == 1)
            report.month_number = stoi(parsed_line[i]);
        else if (i == 2)
        {
            day = stoi(parsed_line[i]);
            if (stoi(parsed_line[i]) == DAY_NUMBER)
                is_month_finished = true;
        }
        else
        {
            month_usages_matrix[day - 1][i - 3] = stoi(parsed_line[i]);
        }
    }
}

void Resource::read_csv_file()
{
    string line;
    fstream file(resource_file_path, ios::in);
    if (file.is_open())
    {
        getline(file, line);
        bool done = false;
        while(true)
        {
            MonthReport report;
            bool is_month_finished = false;
            vector<vector<int>> month_usages_matrix(DAY_NUMBER, vector<int>(HOUR_NUMBER, 0));
            while(true)
            {
                if (getline(file, line).eof())
                {
                    done = true;
                    break;
                }
                parse_one_line(line, is_month_finished, report, month_usages_matrix);
                if (is_month_finished)
                {
                    calc_report_field(month_usages_matrix, report);
                    break;
                }
            }
            if (!done)
                resource_reports.push_back(report);
            else
                break;
        }
        file.close();
    }
}

void Resource::calc_report_field(vector<vector<int>> &month_usages_matrix, MonthReport &report)
{
    report.total_usage = calc_total_usage(month_usages_matrix);
    report.average_usage = calc_average_usage(report.total_usage);
    report.peak_hour = calc_peak_hour(month_usages_matrix);
    report.total_usage_in_peak = calc_total_usage_in_peak(month_usages_matrix, report.peak_hour);
    report.diff_peak_average = calc_difference_from_average_in_peak(report.total_usage, report.peak_hour);
}

int Resource::calc_total_usage(vector<vector<int>> &month_usages_matrix)
{
    int result = 0;
    for (int i = 0; i < DAY_NUMBER; i++)
    {
        for (int j = 0; j < HOUR_NUMBER; j++)
        {
            result += month_usages_matrix[i][j];
        }
    }
    return result;
}

int Resource::calc_average_usage(int whole_usage)
{
    return whole_usage / DAY_NUMBER;
}

int Resource::calc_peak_hour(vector<vector<int>> &month_usages_matrix)
{
    vector<int> total_usage_in_hour = vector<int>(HOUR_NUMBER, 0);
    for (int i = 0; i < DAY_NUMBER; i++)
    {
        for (int j = 0; j < HOUR_NUMBER; j++)
        {
            total_usage_in_hour[j] += month_usages_matrix[i][j];
        }
    }
    return max_ind(total_usage_in_hour);
}

int Resource::calc_total_usage_in_peak(vector<vector<int>> &month_usages_matrix, int max_hour)
{
    int result = 0;
    for (int i = 0; i < DAY_NUMBER; i++)
    {
        result += month_usages_matrix[i][max_hour];
    }
    return result;
}

int Resource::calc_difference_from_average_in_peak(int whole_usage, int max_hour_usage)
{
    return whole_usage - max_hour_usage;
}

int Resource::calc_total_usage_in_min_hours(vector<vector<int>> &month_usages_matrix)
{
    vector<int> total_in_hour = vector<int>(HOUR_NUMBER, 0);
    for (int i = 0; i < DAY_NUMBER; i++)
    {
        for (int j = 0; j < HOUR_NUMBER; j++)
        {
            total_in_hour[j] += month_usages_matrix[i][j];
        }
    }
    int result = 0;
    for (int i = 0; i < DAY_NUMBER; i++)
    {
        result += month_usages_matrix[i][min_ind(total_in_hour)];
    }
    return result;
}

std::vector<MonthReport> Resource::get_resource_reports()
{
    return resource_reports;
}

void Resource::send_data_to_building(string encoded_records)
{
    log_info(RESOURCE_CALCULATOR_LOG_NAME, "resource calculator sent data to building");
    write_ordinary_pipe(encoded_records.c_str(), encoded_records.size(), write_pipe_fd);
}