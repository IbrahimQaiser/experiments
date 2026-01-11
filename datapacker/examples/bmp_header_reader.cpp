/**
 * Example program which reads a BMP file header and displays information about the image
 * Note: All BMP Header formats are not supported, only BITMAPINFOHEADER is supported, so it may be
 * possible that this application fails to work with some BMP files
 */
#include "../include/datapacker.h"
#include <assert.h>
#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./a.out <filename.bmp>" << std::endl;
        exit(1);
    }
    FILE *fp = fopen(argv[1], "rb");
    if (!fp)
    {
        std::cerr << "Could not open image file" << std::endl;
        exit(1);
    }
    uint8_t buffer[54];
    if (fread(buffer, 54, 1, fp) != 1)
    {
        std::cerr << "Could not read BMP header" << std::endl;
        exit(1);
    }
    if (!(buffer[0] == 'B' && buffer[1] == 'M'))
    {
        std::cerr << "Not a BMP file" << std::endl;
        exit(1);
    }

    uint8_t *bufptr = buffer;
    bufptr = bufptr + 2;

    uint32_t size;
    uint32_t reserved;
    uint32_t starting_offset;
    uint32_t dib_header_size;
    int32_t width;
    int32_t height;
    uint16_t color_planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    uint32_t palette_colors;
    uint32_t imp_colors;

    datapacker::bytes::decode_le(bufptr, size, reserved, starting_offset, dib_header_size, width,
                             height, color_planes, bpp, compression, image_size,
                             horizontal_resolution, vertical_resolution, palette_colors,
                             imp_colors);

    std::cout << "BMP File size: " << size << std::endl;
    std::cout << "Starting offset: " << starting_offset << std::endl;
    std::cout << "DIB Header size: " << dib_header_size << std::endl;
    std::cout << "Image width: " << width << std::endl;
    std::cout << "Image height: " << height << std::endl;
    std::cout << "Color planes: " << color_planes << std::endl;
    std::cout << "Bits per pixel: " << bpp << std::endl;
    std::cout << "Compression: " << compression << std::endl;
    std::cout << "Raw image size: " << image_size << std::endl;
    std::cout << "Horizontal resolution: " << horizontal_resolution << std::endl;
    std::cout << "Vertical resolution: " << vertical_resolution << std::endl;
    std::cout << "Palette colors: " << palette_colors << std::endl;
    std::cout << "Important colors: " << imp_colors << std::endl;

    fclose(fp);
}