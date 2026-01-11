#pragma once
#include <iostream>

class Config
{
  private:
    int screen_width_;
    int screen_height_;
    int FPS_;
    int number_boids_;
    int min_speed_;
    int boid_size_;
    int max_speed_;

    int edge_dist_;

    bool enable_debug_;

    float separation_coeff_;
    float alignment_coeff_;
    float cohesion_coeff_;

    int boid_sides_; // For triangle
    int debug_vector_length_;
    int debug_vector_thickness_;

    float fov_angle_;
    float fov_radius_;

    Config()
    {
        screen_width_ = 1920;
        screen_height_ = 1080;
        FPS_ = 60;
        number_boids_ = 160;
        min_speed_ = 100;
        boid_size_ = 8;
        max_speed_ = 200;
        edge_dist_ = 20;
        title = "Boids - Flock of birds";
        enable_debug_ = true;
        boid_sides_ = 3; // For triangle
        debug_vector_length_ = 20;
        debug_vector_thickness_ = 2;
        fov_angle_ = 240; // In degrees
        fov_radius_ = 50;

        separation_coeff_ = 500;
        alignment_coeff_ = 0.1;
        cohesion_coeff_ = 2.0;
    }

  public:
    const char *title;

    const int &screen_width = screen_width_;
    const int &screen_height = screen_height_;
    const int &FPS = FPS_;
    const int &number_boids = number_boids_;
    const int &min_speed = min_speed_;
    const int &boid_size = boid_size_;
    const int &max_speed = max_speed_;
    const int &edge_dist = edge_dist_;

    bool &enable_debug = enable_debug_;

    const int &boid_sides = boid_sides_;
    const int &debug_vector_length = debug_vector_length_;
    const int &debug_vector_thickness = debug_vector_thickness_;

    const float &fov_angle = fov_angle_;
    float &fov_radius = fov_radius_;

    float &separation_coeff = separation_coeff_;
    float &alignment_coeff = alignment_coeff_;
    float &cohesion_coeff = cohesion_coeff_;

    static Config &get()
    {
        static Config config;
        return config;
    }

    static void toggle(bool &b) { b = !b; }

    // Delete copy assignment & copy constructors, also delete move
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    Config(Config &&) = delete;
    Config &operator=(Config &&) = delete;
};