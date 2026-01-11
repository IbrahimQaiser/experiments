// Homepage: https://github.com/ananthvk/cpp-raytracer
// A header file for defining commonly used structures, functions and constants
#pragma once
#include "linalg.h"
#include <limits>
#include <random>
#include <stdlib.h>
constexpr double PI = 3.141592653589793238463;
constexpr double ZERO_EPSILON = 1e-6;
constexpr double EPSILON = 1e-6;

/// @brief Performs linear interpolation
/// @param s starting value
/// @param e ending value
/// @param t parameter
/// @return an interpolated value between start and end
inline double lerp(double s, double e, double t) { return (1 - t) * s + t * e; }

using vec3 = linalg::vec<double, 3>;
const double INF = std::numeric_limits<double>::infinity();

// A class for managing a single ray in a raytracer
class Ray
{
  private:
    vec3 rorigin, rdirection;

  public:
    /// @brief  Creates a new ray with origin at (0,0,0) and direction as (0,0,0)
    Ray() : rorigin(), rdirection() {}

    /// @brief Create a new ray
    /// @param p Origin of the ray
    /// @param d Direction of the ray
    /// @param normalized true if the direction is normalized(default: false)
    Ray(const vec3 p, const vec3 d, bool normalized = false) : rorigin(p)
    {
        if (normalized)
        {
            rdirection = d;
        }
        else
        {
            rdirection = linalg::normalize(d);
        }
    }

    /// @return Direction of this ray
    vec3 direction() const { return rdirection; }

    /// @return Starting point of this ray
    vec3 origin() const { return rorigin; }

    /// @param t Parameter A = p + dt, to find a point on the ray at a distance t
    /// @return Returns a point on this ray at a distance t from the origin of the ray
    vec3 at(double t) const { return rorigin + (t * rdirection); }
};

// A struct which stores some parameters for a ray intersecting with an object
struct Intersection
{
    // Point of intersection with ray.
    // If there are multiple points of intersection, the one with the least
    // distance from the origin of the ray is stored
    double parametric;
    // Point in cartesian system
    vec3 point;
    // The ray for which these values are calculated
    Ray ray;
    // The outward normal at the intersection point
    vec3 o_normal;
    // Local normal at the intersection point, normal at the side on which the
    // ray is present
    vec3 local_normal;
    // true if the intersection occured, otherwise false
    bool occured;
    // id(index) of the material at the intersection point
    int material_id;
    // true if the ray is outside the surface
    bool front;
};

// parameters for the intersect function, ray, t_min, t_max
struct RayParams
{
    // The ray for which intersection has to be calculated
    Ray ray;
    // Minimum value of t which is valid
    double t_min;
    // Maximum value of t which is valid
    double t_max;
};

/// @return A uniformly generated random number between 0 and 1
inline double uniform()
{
    static std::random_device device;
    static std::mt19937 gen(device());
    static std::uniform_real_distribution<double> distribution(0, 1);

    return distribution(gen);
}

/// @return A uniformly generated random number between -1 and 1
inline double uniform_minus_one_to_one()
{
    static std::random_device device;
    static std::mt19937 gen(device());
    static std::uniform_real_distribution<double> distribution(-1, 1);

    return distribution(gen);
}

/// @return A uniformly generated random real number between min and max
inline double uniform(double min, double max)
{
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

/// @return A uniformly generated integer between min and max
inline int randint(int min, int max)
{
    static std::uniform_int_distribution<int> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

/// @return A random unit vector inside a sphere
inline vec3 random_in_unit_sphere()
{
    while (1)
    {
        vec3 v(uniform_minus_one_to_one(), uniform_minus_one_to_one(), uniform_minus_one_to_one());
        if (linalg::length2(v) < 1)
            return linalg::normalize(v);
    }
}

/// @return A random vector inside a disk(circlular plate)
inline vec3 random_in_unit_disk()
{
    while (1)
    {
        vec3 v(uniform_minus_one_to_one(), uniform_minus_one_to_one(), 0);
        if (linalg::length2(v) < 1)
            return v;
    }
}

/// @param v Vector to be checked
/// @return true if the vector is a zero vector
inline bool is_zero_vector(const vec3 v)
{
    return fabs(v.x) < ZERO_EPSILON && fabs(v.y) < ZERO_EPSILON && fabs(v.z) < ZERO_EPSILON;
}

/// @brief Performs reflection using laws of reflection
/// @param v Direction of incident ray
/// @param n Vector representing normal at the surface
/// @return The reflected ray
inline vec3 reflect(const vec3 v, const vec3 n) { return v - 2 * linalg::dot(v, n) * n; }

/// @brief Performs reflection using laws of reflection
/// For more information
/// https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics/snell'slaw
/// @param incident Direction of incident ray
/// @param normal Vector representing normal at the surface
/// @param rel_i Relative refractive index of the surface with its surroundings
/// @return The refracted ray
inline vec3 refract(const vec3 incident, const vec3 normal, double rel_i)
{
    auto cos_theta = std::min(linalg::dot(-incident, normal), 1.0);
    vec3 refracted_perpendicular = rel_i * (incident + cos_theta * normal);
    vec3 refracted_parallel = -sqrt(fabs(1.0 - linalg::length2(refracted_perpendicular))) * normal;
    return refracted_perpendicular + refracted_parallel;
}

/// @brief Schlick approximation for viewing angle of glass
/// The reflectivity of glass changes with the angle in which it is viewed. For example water
/// appears like a mirror when viewed in a particular angle. This is an approximation of the real
/// phenomenon.
/// For more information
/// https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics/schlickapproximation
inline double schlick(double cosine, double ref_idx)
{
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

/// @brief  Checks if the surface will reflect or refract
/// @return true if this results in reflection instead of refraction
inline bool schlick_reflects(double cosine, double refractive_index)
{
    return schlick(cosine, refractive_index) > uniform();
}

/// @brief Check equality between vectors
/// @param v1 The first vector
/// @param v2 The second vector
/// @return true if both vectors are nearly same
inline bool almost_equal(const vec3 v1, const vec3 v2)
{
    return fabs(v1.x - v2.x) < EPSILON && fabs(v1.y - v2.y) < EPSILON &&
           fabs(v1.z - v2.z) < EPSILON;
}

/// @brief Converts degrees to radians
inline constexpr double radians(double degrees) { return (PI / 180.0) * degrees; }