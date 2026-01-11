#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "operations.hpp"
#include "stb_image_write.h"

auto extract(const std::string &image_file_path, Cluster &cluster_algorithm) -> std::vector<Color>
{
    auto img = Image::from_file(image_file_path);
    auto colors = img.get_colors();
    cluster_algorithm.fit(colors);
    return cluster_algorithm.labels();
}

auto apply(const std::string &input_image_path, const std::string &output_image_path,
           const std::vector<Color> &palette) -> void
{
    auto input_img = Image::from_file(input_image_path);
    auto colors = input_img.get_colors();
    auto output_img = Image::create(input_img.width(), input_img.height(), input_img.channels());
    auto kmeans = KMeans::from_palette(palette);

    auto colors_clustered = kmeans.predict(colors);
    auto labels = kmeans.labels();

    unsigned char *buffer = output_img.buffer();
    for (size_t i = 0; i < (input_img.width() * input_img.height()); ++i)
    {
        unsigned char *ptr = buffer + (input_img.channels() * i);
        auto color = labels[colors_clustered[i]];

        ptr[0] = color.r;
        ptr[1] = color.g;
        ptr[2] = color.b;
        if (input_img.channels() == 4)
        {
            ptr[3] = color.a;
        }
    }
    stbi_write_png(output_image_path.c_str(), input_img.width(), input_img.height(),
                   input_img.channels(), buffer, input_img.width() * input_img.channels());
}