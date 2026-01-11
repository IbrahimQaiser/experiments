// Homepage: https://github.com/ananthvk/cpp-raytracer
#include "material.hpp"

LambertianDiffuse::LambertianDiffuse() : albedo(color(0.5, 0.5, 0.5)) {}

LambertianDiffuse::LambertianDiffuse(const color &albedo) : albedo(albedo) {}

MaterialInteraction LambertianDiffuse::interact(const RayParams params,
                                                const Intersection &intersect) const
{
    // Diffuse materials
    // -----------------
    // When light is incident on these materials, the light rays are bounced back randomly in
    // different directions. These materials are rough and do not produce a clear reflection.
    // Examples: plastic, wood, etc.
    MaterialInteraction interaction;
    auto scatter_direction = intersect.local_normal + random_in_unit_sphere();
    // To prevent cases where the direction becomes zero when the random vector is exactly opposite
    // to the normal
    if (is_zero_vector(scatter_direction))
        scatter_direction = intersect.local_normal;
    // If the unit vector and the ray are not on the same side, flip the
    // vector
    if (linalg::dot(intersect.local_normal, scatter_direction) < 0.0)
    {
        scatter_direction = -scatter_direction;
    }
    interaction.additional_rays = true;
    interaction.attenuation = albedo;
    interaction.ray = Ray(intersect.point, scatter_direction);
    return interaction;
}

Metal::Metal() : albedo(color(0.5, 0.5, 0.5)), fuzziness(0.0) {}

Metal::Metal(const color &albedo, double fuzziness)
    : albedo(albedo), fuzziness(fuzziness < 1 ? fuzziness : 1)
{
}

MaterialInteraction Metal::interact(const RayParams params, const Intersection &intersect) const
{
    // Metals
    // ------
    // Metals are highly polished and the reflection of light is well defined and follows the
    // laws of reflection. All rays which are incident with the same angle are reflected
    // with the same angle.
    MaterialInteraction interaction;
    auto reflect_direction = reflect(intersect.ray.direction(), intersect.local_normal);
    auto scattered = linalg::normalize(reflect_direction + fuzziness * random_in_unit_sphere());
    // If the scattered ray passes into the surface, ignore it
    if (linalg::dot(scattered, intersect.local_normal) < 0)
    {
        interaction.additional_rays = false;
        interaction.attenuation = BLACK;
    }
    else
    {
        interaction.additional_rays = true;
        interaction.ray = Ray(intersect.point, scattered);
        interaction.attenuation = albedo;
    }
    return interaction;
}

Glass::Glass() : albedo(WHITE), r_index(1.5) {}

Glass::Glass(const color &albedo, double r_index) : albedo(albedo), r_index(r_index) {}

MaterialInteraction Glass::interact(const RayParams params, const Intersection &intersect) const
{
    MaterialInteraction interaction;
    interaction.attenuation = albedo;
    interaction.additional_rays = true;

    double cos_theta = fmin(linalg::dot(-params.ray.direction(), intersect.local_normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    // Calculate the refractive index
    double ri = intersect.front ? (1.0 / r_index) : r_index;
    // Also apply schlick approximation
    if (ri * sin_theta > 1.0 || schlick_reflects(cos_theta, ri))
    {
        // There is no solution to Snell's law, so only reflection is possible
        auto reflected = reflect(params.ray.direction(), intersect.local_normal);
        interaction.ray = Ray(intersect.point, reflected);
    }
    else
    {
        // The ray gets refracted
        auto refracted = refract(params.ray.direction(), intersect.local_normal, ri);
        interaction.ray = Ray(intersect.point, refracted);
    }

    return interaction;
}