// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include "commons.hpp"
#include "config.h"
#include <iostream>
#include <ostream>
using namespace linalg::ostream_overloads;

// An abstract class which represents a camera and is used to get rays according
// to the pixel coordinates
class Camera
{
  public:
    virtual Ray get_ray(int row, int col, bool sample = false) const = 0;
    virtual void debug_info(std::ostream &os) const = 0;

    virtual ~Camera() {}
};

// A simple camera class which implements depth-of-field, field-of-view, lookat and position
// Note unless specified, all angle values are in radians
class MovableCamera : public Camera
{
  private:
    // A vector which represents the alignment of the camera
    vec3 up;
    // A vector which points to the right of the camera
    vec3 right;
    // The direction the camera is looking at
    vec3 direction;
    // Position of the camera
    vec3 position;
    // Distance between the camera and the viewport screen
    double focal_length;
    // FOV angle
    double fov;
    // Width of the image (in pixels)
    int image_width;
    // Height of the image (in pixels)
    int image_height;
    // Aspect ratio is width / height
    double aspect_ratio;
    // Width of the virtual viewport (in metres)
    double viewport_width;
    // Height of the virtual viewport (in metres)
    double viewport_height;
    // It is the horizontal spacing between two adjacent pixels in the viewport
    double delta_x;
    // It is the vertical spacing between two adjacent pixels in the viewport
    double delta_y;
    // The angle through which the rays are spread out from the origin of the camera
    double defocus_angle;
    // The radius of disk from which rays are cast to the screen
    double defocus_radius;

    /// Returns a random origin for a new ray on the defocus disk
    vec3 get_defocused_origin() const;

  public:
    MovableCamera(const Config &conf);
    /// Returns a ray which passes through a pixel at (row, col)
    /// Note: pixels start from (0,0), which is the top left corner
    /// @param row y coordinate or row of the pixel
    /// @param col x coordinate or column of the pixel
    /// @param sample Randomize the ray or not(by default, false)
    /// @return A ray which passes through the given pixel from the center of the camera
    Ray get_ray(int row, int col, bool sample = false) const override;
    /// Prints debug information to the given stream
    /// @param os - std::ostream object
    void debug_info(std::ostream &os) const override;
};
