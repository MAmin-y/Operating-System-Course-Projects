#include "Filters.hpp"

const Pixel WHITE = Pixel((uint8_t)255, (uint8_t)255, (uint8_t)255);

std::vector<std::vector<double>> BLUR_KERNEL = 
{
    {0.0625, 0.125, 0.0625},
    {0.125, 0.25, 0.125},
    {0.0625, 0.125, 0.0625}
};

std::vector<std::vector<double>> PURPLE_KERNEL = 
{
    {0.5, 0.3, 0.5},
    {0.16, 0.5, 0.16},
    {0.6, 0.2, 0.8}
};

Filters::Filters(BMP* last_image_)
{
    last_image = last_image_;
    height = last_image->get_height();
    width = last_image->get_width();
}

BMP::BMP(const BMP& copy)
    : pixels(copy.pixels),
      bitmap_file_header(copy.bitmap_file_header),
      bitmap_info_header(copy.bitmap_info_header) {}

void Filters::three_by_three_conv(BMP* vertical_mirror_img, int row, int col, const std::vector<std::vector<double>>& kernel) 
{
    double red = 0, blue = 0, green = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j) 
        {
            int cur_row = row + i;
            int cur_col = col + j;
            Pixel curr_image_pixel = vertical_mirror_img->pixels[cur_row][cur_col];
            red += (curr_image_pixel.red) * kernel[i + 1][j + 1];
            blue += (curr_image_pixel.blue) * kernel[i + 1][j + 1];
            green += (curr_image_pixel.green) * kernel[i + 1][j + 1];
        }
    }
    last_image->pixels[row][col] = Pixel(red,green,blue);
}

void Filters::vertical_mirror()
{
    for (int i = 0; i < std::floor(height / 2); ++i) 
    {
        for (int j = 0; j < width; ++j) 
        {
            last_image->vertical_symmetric_swap_pixel(i,j);
        }
    }
}

void Filters::blur() 
{
    BMP* vertical_mirror_img = new BMP(*last_image);

    for (int i = 1; i < height - 1; ++i) 
    {
        for (int j = 1; j < width - 1; ++j) 
        {
            three_by_three_conv(vertical_mirror_img, i, j, BLUR_KERNEL);
        }
    }
    delete vertical_mirror_img;
}

void Filters::make_pixel_purple(int row, int col)
{
    Pixel old_pix = last_image->pixels[row][col];
    Pixel new_pix(
        old_pix.red * PURPLE_KERNEL[0][0] + old_pix.green * PURPLE_KERNEL[0][1] + old_pix.blue * PURPLE_KERNEL[0][2],
        old_pix.red * PURPLE_KERNEL[1][0] + old_pix.green * PURPLE_KERNEL[1][1] + old_pix.blue * PURPLE_KERNEL[1][2],
        old_pix.red * PURPLE_KERNEL[2][0] + old_pix.green * PURPLE_KERNEL[2][1] + old_pix.blue * PURPLE_KERNEL[2][2]
    );
    last_image->pixels[row][col] = new_pix;
}

void Filters::purple_haze()
{
    for (int i = 0; i < height; ++i) 
    {
        for (int j = 0; j < width; ++j) 
        {
            make_pixel_purple(i, j);
        }
    }
}

void Filters::draw_line(int src_x, int src_y, int dest_x, int dest_y, const Pixel& color)
{
    int dx = dest_x - src_x;
    int dy = dest_y - src_y;
    int steps = std::max(abs(dx), abs(dy));
    double x_step = static_cast<double>(dx) / (double)steps;
    double y_step = static_cast<double>(dy) / (double)steps;
    auto x = static_cast<double>(src_x);
    auto y = static_cast<double>(src_y);
    for (int i = 0; i <= steps; ++i)
    {
        last_image->pixels[(int) y][(int) x] = color;
        x += x_step;
        y += y_step;
    }
}

void Filters::diagnal_hatching()
{
    int mid_width = width / 2;
    int mid_height = height / 2;
    draw_line(0, mid_height, mid_width, 0, WHITE);
    draw_line(0, height - 1, width - 1, 0, WHITE);
    draw_line(width - 1, mid_height, mid_width, height - 1, WHITE);
}