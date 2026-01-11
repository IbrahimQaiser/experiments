#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/stb_image_write.h"
#include <iostream>

int main()
{
    const int channels = 3;
    const int width = 1080;
    const int height = 1080;
    unsigned char *data = new unsigned char[width * height * channels]();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // R G
            // B W
            if (i <= height / 2 && j <= width / 2)
            {
                data[(i * width + j) * channels] = 255;
                data[(i * width + j) * channels + 1] = 0;
                data[(i * width + j) * channels + 2] = 0;
            }
            else if (i <= height / 2 && j > width / 2)
            {
                data[(i * width + j) * channels] = 0;
                data[(i * width + j) * channels + 1] = 255;
                data[(i * width + j) * channels + 2] = 0;
            }
            else if (i > height / 2 && j <= width / 2)
            {
                data[(i * width + j) * channels] = 0;
                data[(i * width + j) * channels + 1] = 0;
                data[(i * width + j) * channels + 2] = 255;
            }
            else if (i > height / 2 && j > width / 2)
            {
                data[(i * width + j) * channels] = 255;
                data[(i * width + j) * channels + 1] = 255;
                data[(i * width + j) * channels + 2] = 255;
            }
        }
    }

    stbi_write_png("test.png", width, height, channels, data, width * channels);
    delete[] data;
    return 0;
}