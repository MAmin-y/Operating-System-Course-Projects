#!/bin/bash

arg1=$1

cd Serial
make
cd ..
cd Parallel
make
cd ..

echo "Serial:"
start_serial=$(date +%s.%N)
./Serial/ImageFilters.out "./assets/$arg1"
end_serial=$(date +%s.%N)
execution_time_serial=$(bc -l <<< "$end_serial - $start_serial")


echo "Parallel:"
start_parallel=$(date +%s.%N)
./Parallel/ImageFilters.out "./assets/$arg1"
end_parallel=$(date +%s.%N)
execution_time_parallel=$(bc -l <<< "$end_parallel - $start_parallel")


speedup=$(bc -l <<< "$execution_time_serial / $execution_time_parallel")
echo "Speedup = ${speedup::6}"
