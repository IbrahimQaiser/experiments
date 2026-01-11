#pragma once
#include "image.hpp"
#include "cluster.hpp"

auto extract(const std::string &image_file_path, Cluster &cluster_algorithm) -> std::vector<Color>;

auto apply(const std::string &input_image_path, const std::string &output_image_path,
           const std::vector<Color> &palette) -> void;