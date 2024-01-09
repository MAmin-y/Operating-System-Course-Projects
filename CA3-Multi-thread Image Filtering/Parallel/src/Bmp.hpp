#ifndef BMP_HPP
#define BMP_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

const int NO_THEREADS = 8;

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack(pop)
#pragma pack(push, 1)
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

typedef struct Pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Pixel(double r, double g, double b)
    {
        red = (uint8_t)std::min(std::max<int>(r, 0), 255);
        green = (uint8_t)std::min(std::max<int>(g, 0), 255);
        blue = (uint8_t)std::min(std::max<int>(b, 0), 255);
    }
    Pixel()
    {
        red = (uint8_t)0;
        green = (uint8_t)0;
        blue = (uint8_t)0;
    }
}Pixel;

class BMP {
public:
    BMP() = default;
    BMP(std::istream& read_stream);
    BMP(const BMP& copy);
    int get_height();
    int get_width();
    void vertical_symmetric_swap_pixel(int row, int col);
    void read_bitmap(std::istream& read_stream);
    void write_bitmap(std::string file_name);

    std::vector<std::vector<Pixel>> pixels;

private:
    BITMAPFILEHEADER bitmap_file_header;
    BITMAPINFOHEADER bitmap_info_header;
    int NO_line_bytes;
    int pixel_linear_pos(int row, int col);
    void create_join_threads(int NO_threads, void* (*function)(void*), unsigned char* img_buffer);
    static void* read_partial_image(void* args);
    static void* write_partial_image(void* args);
};

typedef struct Thread_bmp_args
{
    Thread_bmp_args(BMP* bmp_, int begin_, int end_, unsigned char* img_buff_)
        : bmp(bmp_), begin(begin_), end(end_), img_buff(img_buff_) {}
    BMP* bmp;
    int begin;
    int end;
    unsigned char* img_buff;
}Thread_bmp_args;

#endif