// Homepage: https://github.com/ananthvk/cpp-raytracer
// Some configuration options such as image width, height, samples per pixel, etc
#pragma once
#include "commons.hpp"
constexpr int DEFAULT_GAMMA = 2;
constexpr int DEFAULT_IMAGE_WIDTH = 400;
constexpr int DEFAULT_IMAGE_HEIGHT = DEFAULT_IMAGE_WIDTH * (9.0 / 16.0);
constexpr int DEFAULT_PROGRESSBAR_WIDTH = 40;
constexpr int DEFAULT_SAMPLES_PER_PIXEL = 100; // NOTE: MUST BE NON-ZERO AND POSITIVE
constexpr int DEFAULT_RECURSION_LIMIT = 50;
constexpr double DEFAULT_CAMERA_FOV = radians(20);
constexpr vec3 DEFAULT_CAMERA_POSITION = vec3(13, 2, 3);
constexpr vec3 DEFAULT_CAMERA_LOOKAT = vec3(0, 0, 0);
constexpr vec3 DEFAULT_CAMERA_UP = vec3(0, 1, 0);
constexpr double DEFAULT_CAMERA_DEFOCUS_ANGLE = radians(0.6);
constexpr double DEFAULT_CAMERA_FOCAL_LENGTH = 10.0;
constexpr char *DEFAULT_FILENAME = "output2.png";

// Configuration settings for this render
struct Config
{
  public:
    // Some default settings
    int gamma = DEFAULT_GAMMA;
    int image_width = DEFAULT_IMAGE_WIDTH;
    int image_height = DEFAULT_IMAGE_HEIGHT;
    int progressbar_width = DEFAULT_PROGRESSBAR_WIDTH;
    int samples_per_pixel = DEFAULT_SAMPLES_PER_PIXEL; // NOTE: MUST BE NON-ZERO AND POSITIVE
    int recursion_limit = DEFAULT_RECURSION_LIMIT;
    double camera_fov = DEFAULT_CAMERA_FOV;
    vec3 camera_position = DEFAULT_CAMERA_POSITION;
    vec3 camera_lookat = DEFAULT_CAMERA_LOOKAT;
    vec3 camera_up = DEFAULT_CAMERA_UP;
    double camera_defocus_angle = DEFAULT_CAMERA_DEFOCUS_ANGLE;
    double camera_focal_length = DEFAULT_CAMERA_FOCAL_LENGTH;
    std::string filename = DEFAULT_FILENAME;
};
