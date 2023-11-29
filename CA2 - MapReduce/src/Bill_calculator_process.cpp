#include "Bill_calculator.hpp"

int main(int argc, char const *argv[])
{
    Bill_Calculator bill_calculator = Bill_Calculator(stoi(argv[1]));
    bill_calculator.listen();
}