#include "Bmp.hpp"
#include <pthread.h>


BMP::BMP(std::istream& read_stream)
{
    read_bitmap(read_stream);
}

BMP::BMP(const BMP& copy)
    : pixels(copy.pixels),
      bitmap_file_header(copy.bitmap_file_header),
      bitmap_info_header(copy.bitmap_info_header) {}

void BMP::create_join_threads(int NO_threads, void* (*function)(void*), unsigned char* img_buffer)
{
    std::vector<pthread_t> thread_ids;
    for(int i=0; i<=NO_threads; i++)
    {
        pthread_t thread_id;
        if (bitmap_info_header.biHeight % NO_threads && i == NO_threads) 
        {
            Thread_bmp_args* args = new Thread_bmp_args(this, (bitmap_info_header.biHeight - 
                (bitmap_info_header.biHeight % NO_threads)), bitmap_info_header.biHeight, img_buffer);
            
            pthread_create(&thread_id, NULL, function, (void*)args);
            thread_ids.push_back(thread_id);
        }
        else if(i != NO_threads)
        {
            Thread_bmp_args* args = new Thread_bmp_args(this, i * (bitmap_info_header.biHeight / NO_threads),
                (i + 1) * (bitmap_info_header.biHeight / NO_threads), img_buffer);

            pthread_create(&thread_id, NULL, function, (void*)args);
            thread_ids.push_back(thread_id);
        }
    }
    for (int i=0; i<thread_ids.size(); i++)
        pthread_join(thread_ids[i], NULL);
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

    create_join_threads(NO_THEREADS, read_partial_image, img_buffer);

    delete[] img_buffer;
}

void* BMP::read_partial_image(void* args)
{
    Thread_bmp_args* arguments = (struct Thread_bmp_args *) args;
    for (int i = arguments->begin; i < arguments->end; ++i) 
    {
        for (int j = 0; j < arguments->bmp->get_width(); ++j) 
        {
            unsigned char* temp_pixel = &arguments->img_buff[arguments->bmp->pixel_linear_pos(i, j)];
            arguments->bmp->pixels[i][j] = Pixel((uint8_t)temp_pixel[2],(uint8_t)temp_pixel[1],(uint8_t)temp_pixel[0]);
        }
    }
    return NULL;
}

void BMP::write_bitmap(std::string file_name) 
{
    std::ofstream write_stream(file_name);
    write_stream.write((char*)(&bitmap_file_header), sizeof(BITMAPFILEHEADER));
    write_stream.write((char*)(&bitmap_info_header), sizeof(BITMAPINFOHEADER));

    int cur_stream_point = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    while (cur_stream_point < bitmap_file_header.bfOffBits) 
    {
        write_stream << '\0';
        ++cur_stream_point;
    }
    
    unsigned char* img_buffer = new unsigned char[bitmap_info_header.biSizeImage];

    create_join_threads(NO_THEREADS, write_partial_image, img_buffer);

    write_stream.write((char*)img_buffer, bitmap_info_header.biSizeImage);
    delete[] img_buffer;
    write_stream.close();
}

void* BMP::write_partial_image(void* args)
{
    Thread_bmp_args* arguments = (struct Thread_bmp_args *) args;
    for (int i = arguments->begin; i < arguments->end; ++i) 
    {
        for (int j = 0; j < arguments->bmp->get_width(); ++j) 
        {
            unsigned char* temp_pixel = &arguments->img_buff[arguments->bmp->pixel_linear_pos(i, j)];
            temp_pixel[0] = arguments->bmp->pixels[i][j].blue;
            temp_pixel[1] = arguments->bmp->pixels[i][j].green;
            temp_pixel[2] = arguments->bmp->pixels[i][j].red;
        }
    }
    return NULL;
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