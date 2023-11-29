#include "Utils.hpp"

using namespace std;


vector<string> tokenize(string s, string del)
{
    vector<string> parsed;
    int start, end = -1*del.size();
    do {
        start = end + del.size();
        end = s.find(del, start);
        parsed.push_back(s.substr(start, end - start));
    } while (end != -1);
    return parsed;
}

int max_ind(const vector<int> &numbers)
{
    int max_ind = 0;
    int max_val = numbers[0];
    for (int i = 1; i < int(numbers.size()); i++)
    {
        if (numbers[i] > max_val)
        {
            max_val = numbers[i];
            max_ind = i;
        }
    }
    return max_ind;
}

int min_ind(const vector<int> &numbers)
{
    int min_ind = 0;
    int min_val = numbers[0];

    for (int i = 1; i < int(numbers.size()); i++)
    {
        if (numbers[i] < min_val)
        {
            min_val = numbers[i];
            min_ind = i;
        }
    }

    return min_ind;
}

string month_report_encoder(const vector<MonthReport> &reports)
{
    string reports_str;
    for (const MonthReport &report : reports)
    {
        reports_str += to_string(report.year_number) + COMMA_DELIM +
                        to_string(report.month_number) + COMMA_DELIM +
                        to_string(report.total_usage) + COMMA_DELIM +
                        to_string(report.average_usage) + COMMA_DELIM +
                        to_string(report.peak_hour) + COMMA_DELIM +
                        to_string(report.total_usage_in_peak) + COMMA_DELIM + 
                        to_string(report.diff_peak_average) + FORWARD_SLASH_DELIM;
    }
    reports_str.pop_back();
    return reports_str;
}

vector<MonthReport> month_report_decoder(const string &encodedString)
{
    vector<MonthReport> reports;
    vector<string> all_month_report = tokenize(encodedString, FORWARD_SLASH_DELIM);
    for (int i = 0; i < int(all_month_report.size()); i++)
    {
        MonthReport report;
        vector<string> one_report = tokenize(all_month_report[i], COMMA_DELIM);
        report.year_number = stoi(one_report[0]);
        report.month_number = stoi(one_report[1]);
        report.total_usage = stoi(one_report[2]);
        report.average_usage = stoi(one_report[3]);
        report.peak_hour = stoi(one_report[4]);
        report.total_usage_in_peak = stoi(one_report[5]);
        report.diff_peak_average = stoi(one_report[6]);
        reports.push_back(report);
    }
    return reports;
}

string get_month_report_string(const MonthReport &report, string report_type)
{
    string result;
    result += "\tYear: " + to_string(report.year_number) + "\n";
    result += "\tMonth: " + to_string(report.month_number) + "\n";
    if (report_type == TOTAL_USAGE_REPORT_TYPE)
        result += "\t" + TOTAL_USAGE_REPORT_TYPE + COLON_DELIM + SPACE_DELIM + to_string(report.total_usage) + "\n\n";
    else if (report_type == AVERAGE_USAGE_REPORT_TYPE)
        result += "\t" + AVERAGE_USAGE_REPORT_TYPE + COLON_DELIM + SPACE_DELIM  + to_string(report.average_usage) + "\n\n";
    else if (report_type == PEAK_HOURS_REPORT_TYPE)
        result += "\t" + PEAK_HOURS_REPORT_TYPE + COLON_DELIM + SPACE_DELIM + to_string(report.peak_hour) + "\n\n";
    else if (report_type == DIFF_PEAK_AVERAGE_REPORT_TYPE)
        result += "\t" + DIFF_PEAK_AVERAGE_REPORT_TYPE + COLON_DELIM + SPACE_DELIM + to_string(report.diff_peak_average) + "\n";
    else if (report_type == BILL_REPORT_TYPE)
        result += "\t" + BILL_REPORT_TYPE + COLON_DELIM + SPACE_DELIM + to_string(report.bill) + "\n\n";
    return result;
}

string encode_report_for_main(vector<MonthReport> electricity, vector<MonthReport> water, 
                            vector<MonthReport> gas, string report_requested)
{
    string water_record_str = WATER_RESOURCE + PERCENT_DELIM;
    for (int i = 0; i < int(water.size()); i++)
    {
        water_record_str += get_month_report_string(water[i], report_requested);
        water_record_str += FORWARD_SLASH_DELIM;
    }
    if(!water.size())
    {
        water_record_str += EMPTY_RESOURCE + FORWARD_SLASH_DELIM;
    }
    water_record_str.pop_back();
    water_record_str += PERCENT_DELIM;

    string gas_record_str = GAS_RESOURCE + PERCENT_DELIM;
    for (int i = 0; i < int(gas.size()); i++)
    {
        gas_record_str += get_month_report_string(gas[i], report_requested);
        gas_record_str += FORWARD_SLASH_DELIM;
    }
    if(!gas.size())
    {
        gas_record_str += EMPTY_RESOURCE + FORWARD_SLASH_DELIM;
    }
    gas_record_str.pop_back();
    gas_record_str += PERCENT_DELIM;

    string elec_record_str = ELECTRICITY_RESOURCE + PERCENT_DELIM;
    for (int i = 0; i < int(electricity.size()); i++)
    {
        elec_record_str += get_month_report_string(electricity[i], report_requested);
        elec_record_str += FORWARD_SLASH_DELIM;
    }
    if(!electricity.size())
    {
        elec_record_str += EMPTY_RESOURCE + FORWARD_SLASH_DELIM;
    }
    elec_record_str.pop_back();
    elec_record_str += PERCENT_DELIM;

    return water_record_str + gas_record_str + elec_record_str;
}