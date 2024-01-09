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
    width = last_image->get_width();
    height = last_image->get_height();
}

void Filters::create_join_threads(int limit, int NO_threads, void* (*function)(void*))
{
    std::vector<pthread_t> thread_ids;
    for(int i=0; i<=NO_threads; i++)
    {
        if (limit % NO_threads && i == NO_threads) 
        {
            Thread_filter_args* args = new Thread_filter_args(this, (limit - (limit % NO_threads)), limit);
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, function, (void*)args);
            thread_ids.push_back(thread_id);
        }
        else if(i != NO_threads)
        {
            Thread_filter_args* args = new Thread_filter_args(this, i * (limit / NO_threads), (i + 1) * (limit / NO_threads));
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, function, (void*)args);
            thread_ids.push_back(thread_id);
        }
    }    
    for (int i=0; i<thread_ids.size(); i++)
        pthread_join(thread_ids[i], NULL);
}

void* Filters::partial_vertical_mirror(void* args)
{
    Thread_filter_args* arguments = (struct Thread_filter_args *) args;
    for (int i = 0; i < std::floor(arguments->filters->height / 2); ++i) 
    {
        for (int j = arguments->begin; j < arguments->end; ++j)
        {
            arguments->filters->last_image->vertical_symmetric_swap_pixel(i,j);
        }
    }
    return NULL;
}

void Filters::vertical_mirror()
{
    create_join_threads(width, NO_THEREADS, partial_vertical_mirror);
}

void* Filters::partial_blur(void* args)
{
    Thread_filter_args* arguments = (struct Thread_filter_args *) args;
    for (int i = arguments->begin; i < arguments->end; ++i)
    {
        if(i != 0 && i != arguments->filters->height - 1)
        {
            for (int j = 1; j < arguments->filters->width - 1; ++j) 
            {
                double red = 0, blue = 0, green = 0;
                for (int k = -1; k <= 1; ++k) 
                {
                    for (int l = -1; l <= 1; ++l) 
                    {
                        int cur_row = i + k;
                        int cur_col = j + l;
                        Pixel curr_image_pixel = arguments->filters->vertical_mirror_image->pixels[cur_row][cur_col];
                        red += (curr_image_pixel.red) * BLUR_KERNEL[k + 1][l + 1];
                        blue += (curr_image_pixel.blue) * BLUR_KERNEL[k + 1][l + 1];
                        green += (curr_image_pixel.green) * BLUR_KERNEL[k + 1][l + 1];
                    }
                }
                arguments->filters->last_image->pixels[i][j] = Pixel(red,green,blue);
            }
        }
    }
    return NULL;
}

void Filters::blur() 
{
    vertical_mirror_image = new BMP(*last_image);
    create_join_threads(height, NO_THEREADS, partial_blur);
    delete vertical_mirror_image;
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

void* Filters::partial_purple_haze(void* args)
{
    Thread_filter_args* arguments = (struct Thread_filter_args *) args;
    for (int i = arguments->begin; i < arguments->end; ++i) 
    {
        for (int j = 0; j < arguments->filters->width; ++j) 
        {
            arguments->filters->make_pixel_purple(i, j);
        }
    }
    return NULL;
}

void Filters::purple_haze()
{
    create_join_threads(height, NO_THEREADS, partial_purple_haze);
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