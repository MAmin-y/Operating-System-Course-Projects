#ifndef BMP_HPP
#define BMP_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

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
};

#endif