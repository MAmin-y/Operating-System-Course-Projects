#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "Bmp.hpp"

class Filters {
private:
    BMP* last_image;
    int height;
    int width;
    void three_by_three_conv(BMP* vertical_mirror_img, int row, int col, const std::vector<std::vector<double>>& kernel);
    void make_pixel_purple(int row, int col);
    void draw_line(int x1, int y1, int x2, int y2, const Pixel& color);
    
public:
    Filters(BMP* last_image_);
    void vertical_mirror();
    void blur();
    void purple_haze();
    void diagnal_hatching();
};  


#endif