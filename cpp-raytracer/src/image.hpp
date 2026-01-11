// Homepage: https://github.com/ananthvk/cpp-raytracer
// Function to write images
#pragma once
#include "colors.hpp"
#include "commons.hpp"
#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

const int CHANNELS = 3;
const double SCALE_FACTOR = 255.99;
// Pixels are represented in floating point format, with values from 0 to 1.0
// corresponding to 0 to 255
using image_row = std::vector<color>;
using image = std::vector<image_row>;
// Writes the floating point image data to the file after converting it to
// integer format.
void write_to_file(const std::string &filename, const image& img);
