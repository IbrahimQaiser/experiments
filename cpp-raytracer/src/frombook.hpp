// Homepage: https://github.com/ananthvk/cpp-raytracer
// The code below has been taken from the book to help debug or find bugs
#include "camera.hpp"
#include <ostream>

class ReferenceCamera : public Camera
{
  public:
    double aspect_ratio = 16.0 / 9.0; // Ratio of image width over height
    int image_width = 400;            // Rendered image width in pixel count
    int samples_per_pixel = 100;      // Count of random samples for each pixel
    int max_depth = 50;               // Maximum number of ray bounces into scene

    double vfov = 20;               // Vertical view angle (field of view)
    vec3 lookfrom = vec3(-2, 2, 1); // Point camera is looking from
    vec3 lookat = vec3(0, 0, -1);   // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);       // Camera-relative "up" direction

    double defocus_angle = 10; // Variation angle of rays through each pixel
    double focus_dist = 3.4;   // Distance from camera lookfrom point to plane of perfect focus

  public:
    ReferenceCamera() { initialize(); }

    int image_height;    // Rendered image height
    vec3 center;         // Camera center
    vec3 pixel00_loc;    // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below
    vec3 u, v, w;        // Camera frame basis vectors
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = linalg::normalize(lookfrom - lookat);
        std::cerr << "w:" << w << std::endl;
        u = linalg::normalize(linalg::cross(vup, w));
        std::cerr << "u:" << u << std::endl;
        v = linalg::cross(w, u);
        std::cerr << "v:" << v << std::endl;
        std::cerr << "viewport h:" << viewport_height << std::endl;
        std::cerr << "viewport w:" << viewport_width << std::endl;

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors to the next pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        std::cerr << "delta_u:" << pixel_delta_u << std::endl;
        std::cerr << "delta_v:" << pixel_delta_v << std::endl;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
        std::cerr << "Defocus angle:" << defocus_angle << std::endl;
        std::cerr << "Defocus radius:" << defocus_radius << std::endl;
        std::cerr << "Defocus disk u:" << defocus_disk_u << std::endl;
        std::cerr << "Defocus disk v:" << defocus_disk_v << std::endl;
    }

    Ray get_ray(int j, int i, bool sample) const
    {
        // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
        // the camera defocus disk.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    vec3 pixel_sample_square() const
    {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + uniform();
        auto py = -0.5 + uniform();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    vec3 pixel_sample_disk(double radius) const
    {
        // Generate a sample from the disk of given radius around a pixel at the origin.
        auto p = radius * random_in_unit_disk();
        return (p[0] * pixel_delta_u) + (p[1] * pixel_delta_v);
    }

    vec3 defocus_disk_sample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    void debug_info(std::ostream &os) const { os << "DEBUG" << std::endl; }
};