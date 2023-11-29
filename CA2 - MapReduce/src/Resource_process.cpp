#include "Resource.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        exit(EXIT_FAILURE);
    }
    
    Resource resource = Resource(argv[1], stoi(argv[2]), ResourceType(stoi(argv[3])));
    resource.read_csv_file();
    string encoded_records = argv[4];
    encoded_records += "@";
    encoded_records +=  month_report_encoder(resource.get_resource_reports());
    resource.send_data_to_building(encoded_records);
}