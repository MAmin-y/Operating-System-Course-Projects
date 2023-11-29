#include "main.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        exit(EXIT_FAILURE);
    }
    string buildings_dir_path = argv[1];
    Main main = Main(argv[1]);
    main.log_all_buildings_names();
    main.get_user_request();
    main.make_bill_calculato();
    main.make_buildings();
    main.send_user_request_to_buildings();
    main.wait_for_all_children();
    main.write_output();
}