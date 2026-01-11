// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include "colors.hpp"
#include "commons.hpp"

struct MaterialInteraction
{
    // true if this interaction produces other rays which have to be processed
    bool additional_rays;
    // Additional rays produced due to this interaction
    Ray ray;
    // Color due to this material
    color attenuation;
};

// An abstract class for any material
class Material
{
  public:
    virtual MaterialInteraction interact(const RayParams params,
                                         const Intersection &intersect) const = 0;

    virtual ~Material() {}
};

class LambertianDiffuse : public Material
{
  public:
    // By default the color of the material is gray, (0.5, 0.5, 0.5)
    LambertianDiffuse();

    LambertianDiffuse(const color &albedo);

    MaterialInteraction interact(const RayParams params,
                                 const Intersection &intersect) const override;

  private:
    // Albedo or color of this material
    color albedo;
};

class NormalShader : public Material
{
  public:
    MaterialInteraction interact(const RayParams params,
                                 const Intersection &intersect) const override
    {

        // Shade the normals
        MaterialInteraction interaction;
        auto v = 0.5 * (intersect.local_normal + vec3(1, 1, 1));
        interaction.additional_rays = false;
        interaction.attenuation = v;
        return interaction;
    }
};

class Metal : public Material
{
  public:
    // By default the color of the material is gray
    Metal();

    Metal(const color &albedo, double fuzziness = 0.0);

    MaterialInteraction interact(const RayParams params,
                                 const Intersection &intersect) const override;

  private:
    // Albedo or color of this material
    color albedo;
    double fuzziness;
};

class Glass : public Material
{
  public:
    Glass();
    Glass(const color &albedo, double r_index = 1.5);
    MaterialInteraction interact(const RayParams params,
                                 const Intersection &intersect) const override;

  private:
    // Albedo or color of this material
    color albedo;
    double r_index;
};