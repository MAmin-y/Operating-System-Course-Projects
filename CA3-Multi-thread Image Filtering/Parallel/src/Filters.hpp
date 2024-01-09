#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "Bmp.hpp"

class Filters {

public:
    Filters(BMP* last_image_);
    void vertical_mirror();
    void blur();
    void purple_haze();
    void diagnal_hatching();

private:
    BMP* last_image;
    BMP* vertical_mirror_image;
    int width;
    int height;
    void vertical_symmetric_swap_pixel(int row, int col);
    void make_pixel_purple(int row, int col);
    void draw_line(int x1, int y1, int x2, int y2, const Pixel& color);
    void create_join_threads(int limit, int NO_threads, void* (*function)(void*));

    static void* partial_vertical_mirror(void* args);
    static void* partial_blur(void* args);
    static void* partial_purple_haze(void* args);
};

typedef struct Thread_filter_args
{
    Thread_filter_args(Filters* filters_, int begin_, int end_)
    : filters(filters_), begin(begin_), end(end_) {}
    Filters* filters;
    int begin;
    int end;
}Thread_filter_args;


#endif