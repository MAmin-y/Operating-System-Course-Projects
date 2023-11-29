#include "Consts.hpp"
#include <string>
#include <vector>

vector<string> tokenize(string s, string del);

int max_ind(const vector<int> &numbers);

int min_ind(const vector<int> &numbers);

string month_report_encoder(const vector<MonthReport> &month_report);

vector<MonthReport> month_report_decoder(const string &encodedString);

string get_month_report_string(const MonthReport &month_report, string report_type);

string encode_report_for_main(vector<MonthReport> electricity, vector<MonthReport> water, 
                            vector<MonthReport> gas, string report_requested);