// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include "colors.hpp"
#include "commons.hpp"
#include "material.hpp"
#include "objects.hpp"

class Scene
{
  private:
    std::vector<Object *> objects;
    std::vector<Material *> materials;

  public:
    Scene();

    /// @param ray Input ray
    /// @param recursion_limit Number of times this ray can bounce, after every bounce it is
    /// decreased by one
    /// @return The color of the ray when it passes through this scene
    color color_at(const Ray &ray, int recursion_limit) const;

    /// @param params Ray parameters
    /// @return The intersection which is closest to the ray's origin
    Intersection closest_intersect(const RayParams &params) const;
    ~Scene();
};