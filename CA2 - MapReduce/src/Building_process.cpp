#include "Building.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        exit(EXIT_FAILURE);
    }
    Building building = Building(argv[1], atoi(argv[2]), atoi(argv[3]));
    building.get_main_requests();
    building.make_resource_processes();
    building.wait_for_all_children();
    building.collect_resource_reports();
    building.send_report_to_bill_calculator();
    building.write_result_for_main();
}
