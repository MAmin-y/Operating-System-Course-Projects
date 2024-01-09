#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>

#include "Filters.hpp"

int main(int argc, char const* argv[]) {
    auto start_clk = std::chrono::high_resolution_clock::now();

    std::string file_name(argv[1]);
    std::ifstream input_file(file_name);

    auto read_start_clk = std::chrono::high_resolution_clock::now();
    BMP bmp(input_file);
    auto read_end_clk = std::chrono::high_resolution_clock::now();
    // bmp.write_bitmap("output/original.bmp");
    Filters filters = Filters(&bmp);
    input_file.close();

    auto vertical_mirror_start_clk = std::chrono::high_resolution_clock::now();
    filters.vertical_mirror();
    auto vertical_mirror_end_clk = std::chrono::high_resolution_clock::now();
    // bmp.write_bitmap("output/vertical_mirror.bmp");

    auto blur_start_clk = std::chrono::high_resolution_clock::now();
    filters.blur();
    auto blur_end_clk = std::chrono::high_resolution_clock::now();
    // bmp.write_bitmap("output/blur.bmp");

    auto purple_haze_start_clk = std::chrono::high_resolution_clock::now();
    filters.purple_haze();
    auto purple_haze_end_clk = std::chrono::high_resolution_clock::now();
    // bmp.write_bitmap("output/purple_haze.bmp");

    auto diagnal_hatching_start_clk = std::chrono::high_resolution_clock::now();
    filters.diagnal_hatching();
    auto diagnal_hatching_end_clk = std::chrono::high_resolution_clock::now();

    auto write_start_clk = std::chrono::high_resolution_clock::now();
    bmp.write_bitmap("output/diagnal_hatching.bmp");
    auto write_end_clk = std::chrono::high_resolution_clock::now();

    auto end_clk = std::chrono::high_resolution_clock::now();

    auto read_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (read_end_clk - read_start_clk);

    auto vertical_mirror_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (vertical_mirror_end_clk - vertical_mirror_start_clk);

    auto blur_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (blur_end_clk - blur_start_clk);

    auto purple_haze_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (purple_haze_end_clk - purple_haze_start_clk);
    
    auto diagnal_hatching_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (diagnal_hatching_end_clk - diagnal_hatching_start_clk);

    auto write_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (write_end_clk - write_start_clk);

    auto total_time_taken = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>
    (end_clk - start_clk);

    std::cout << "Reading execution time: " << read_time_taken.count() << " ms" << '\n';
    std::cout << "Vertical Mirror filter execution time: " << vertical_mirror_time_taken.count() << " ms"  << '\n';
    std::cout << "Blur filter execution time: " << blur_time_taken.count() << " ms" << '\n';
    std::cout << "Purple Haze execution time: " << purple_haze_time_taken.count() << " ms" << '\n';
    std::cout << "Diagnal Hatching filter execution time: " << diagnal_hatching_time_taken.count() << " ms" << '\n';
    std::cout << "Writing execution time: " << write_time_taken.count() << " ms" << '\n';
    std::cout << "Total execution time: " << total_time_taken.count() << " ms" << '\n';
}
