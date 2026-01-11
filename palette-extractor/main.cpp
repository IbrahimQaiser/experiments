#include "cluster.hpp"
#include "image.hpp"
#include "operations.hpp"
#include <fstream>
#include <iostream>
#include <string.h>
#define NUMBER_OF_ITERATIONS 100
using std::swap;

auto help_message() -> void
{
    std::cerr << "Usage:\npalette extract <number_of_colors> <output_palette_file> <input_image>"
              << std::endl;
    std::cerr << "palette apply <palette_file> <output_image> <input_image>" << std::endl;
}

auto main(int argc, char *argv[]) -> int
{
    try
    {
        if (argc <= 2)
        {
            help_message();
            return 1;
        }
        if (std::string(argv[1]) == "extract" && argc == 5)
        {
            int number_of_colors = std::stoi(argv[2]);
            KMeans kmeans(number_of_colors, NUMBER_OF_ITERATIONS);
            auto palette = extract(argv[4], kmeans);
            // TOOD: Some error checks omitted, add it later
            std::ofstream output_palette_file(argv[3]);
            if (!output_palette_file)
            {
                std::cerr << "Error while writing palette file: " << strerror(errno) << std::endl;
                return 1;
            }
            // First write the number of components, i.e. RGB or RGBA
            output_palette_file << ((palette[0].has_alpha) ? 4 : 3) << std::endl;
            for (const auto &color : palette)
            {
                output_palette_file << +color.r << " " << +color.g << " " << +color.b;
                if (color.has_alpha)
                    output_palette_file << " " << +color.a;
                output_palette_file << std::endl;
            }
        }
        else if (std::string(argv[1]) == "apply" && argc == 5)
        {
            std::ifstream input_palette_file(argv[2]);
            if (!input_palette_file)
            {
                std::cerr << "Error while reading palette file: " << strerror(errno) << std::endl;
                return 1;
            }
            std::string line;
            std::vector<Color> palette;
            // Read number of components
            std::getline(input_palette_file, line);
            std::stringstream s(line);
            int n_components;
            s >> n_components;

            while (std::getline(input_palette_file, line))
            {
                if (line.empty())
                    continue;
                int r, g, b;
                std::stringstream ss(line);
                // TODO Handle alpha channel
                ss >> r >> g >> b;
                Color c;
                c.r = static_cast<unsigned char>(r);
                c.g = static_cast<unsigned char>(g);
                c.b = static_cast<unsigned char>(b);
                if (n_components == 4)
                {
                    int a;
                    ss >> a;
                    c.a = static_cast<unsigned char>(a);
                    c.has_alpha = true;
                }
                palette.push_back(c);
            }
            apply(argv[4], argv[3], palette);
        }
        else if (std::string(argv[1]) == "help")
        {
            help_message();
            return 0;
        }
        else
        {
            help_message();
            return 1;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}