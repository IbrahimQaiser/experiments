// Homepage: https://github.com/ananthvk/cpp-raytracer
#pragma once
#include "camera.hpp"
#include "colors.hpp"
#include "config.h"
#include "image.hpp"
#include "scene.hpp"

class Renderer
{
  private:
    Config config;

  public:
    Renderer(const Config &config);
    image render(const Camera &cam, const Scene &scene, bool show_progress = true) const;
    void set_config(const Config cfg);
    Config get_config() const;
};

image multi_threaded_render(const Config &cfg, const Camera &cam, const Scene &scene, int number_of_threads);