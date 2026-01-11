// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include "commons.hpp"
#include <vector>

// A class to represent any object which can be rendered on the screen
class Object
{
  public:
    virtual Intersection intersect(const RayParams &params) const = 0;

    virtual ~Object() {}
};

// A class for a sphere object, has center, radius and material as parameters
class Sphere : public Object
{
  private:
    vec3 center;
    double radius;
    int material_id;

  public:
    /// @brief  Default constructor, the sphere is at (0, 0, 0) with radius 0
    Sphere();

    /// @brief Create a new sphere given the material
    /// @param center Center of the sphere
    /// @param radius Radius of the sphere
    /// @param material_id index of the material of the sphere in the materials array/vector
    Sphere(vec3 center, double radius, int material_id);

    /// @brief Finds the intersection between this sphere and the given ray
    /// @param RayParams Ray parameters, such as the ray, minimum allowed t and max allowed t
    Intersection intersect(const RayParams &params) const;

    /// @brief Sets the material of the sphere from the materials array/vector
    void set_material_id(int id);
};
