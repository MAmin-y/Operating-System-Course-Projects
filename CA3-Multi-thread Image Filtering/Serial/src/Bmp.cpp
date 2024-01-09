#include "Bmp.hpp"

BMP::BMP(std::istream& read_stream)
{
    read_bitmap(read_stream);
}

void BMP::read_bitmap(std::istream& read_stream) 
{

    read_stream.read((char*)(&bitmap_file_header), sizeof(BITMAPFILEHEADER));

    if (bitmap_file_header.bfType != 0x4D42)
        throw std::runtime_error("File format is not BMP!");

    read_stream.read((char*)(&bitmap_info_header), sizeof(BITMAPINFOHEADER));
    NO_line_bytes = std::ceil((3 * bitmap_info_header.biWidth) / 4) * 4;

    read_stream.seekg(bitmap_file_header.bfOffBits, std::ios::beg);
    unsigned char* img_buffer = new unsigned char[bitmap_info_header.biSizeImage];
    read_stream.read((char*)img_buffer, bitmap_info_header.biSizeImage);

    pixels.assign(bitmap_info_header.biHeight, std::vector<Pixel>(bitmap_info_header.biWidth));
    for (int i = 0; i < bitmap_info_header.biHeight; ++i) {
        for (int j = 0; j < bitmap_info_header.biWidth; ++j) {
            unsigned char* temp_pixel = &img_buffer[pixel_linear_pos(i, j)];
            pixels[i][j] = Pixel((uint8_t)temp_pixel[2],(uint8_t)temp_pixel[1],(uint8_t)temp_pixel[0]);
        }
    }
    delete[] img_buffer;
}

void BMP::write_bitmap(std::string file_name) 
{
    std::ofstream write_stream(file_name);
    write_stream.write((char*)(&bitmap_file_header), sizeof(BITMAPFILEHEADER));
    write_stream.write((char*)(&bitmap_info_header), sizeof(BITMAPINFOHEADER));

    int cur_stream_point = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    while (cur_stream_point < bitmap_file_header.bfOffBits) {
        write_stream << '\0';
        ++cur_stream_point;
    }
    
    char* img_buffer = new char[bitmap_info_header.biSizeImage];
    for (int i = 0; i < bitmap_info_header.biHeight; ++i) {
        for (int j = 0; j < bitmap_info_header.biWidth; ++j) {
            char* temp_pixel = &img_buffer[pixel_linear_pos(i, j)];
            temp_pixel[0] = pixels[i][j].blue;
            temp_pixel[1] = pixels[i][j].green;
            temp_pixel[2] = pixels[i][j].red;
        }
    }

    write_stream.write(img_buffer, bitmap_info_header.biSizeImage);
    delete[] img_buffer;
    write_stream.close();
}

int BMP::pixel_linear_pos(int row, int col) 
{
    int height = bitmap_info_header.biHeight - row - 1;
    int width = col * 3;
    return height * NO_line_bytes + width;
}

void BMP::vertical_symmetric_swap_pixel(int row, int col)
{
    Pixel temp = pixels[row][col];
    pixels[row][col] = pixels[bitmap_info_header.biHeight - row - 1][col];
    pixels[bitmap_info_header.biHeight - row - 1][col] = temp;
}

int BMP::get_height() 
{
    return bitmap_info_header.biHeight;
}

int BMP::get_width()
{
    return bitmap_info_header.biWidth;
}