// Homepage: https://github.com/ananthvk/cpp-raytracer
#include "camera.hpp"
#include "colors.hpp"
#include "config.h"
#include "frombook.hpp"
#include "image.hpp"
#include "progressbar.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
#include <functional>
#include <iostream>
#include <thread>

void gammacorrect(image &img, int gamma)
{
    for (auto &i : img)
    {
        for (auto &j : i)
        {
            j = gamma_correction(j, gamma);
        }
    }
}

int main()
{
    // TODO: Modify the random code so that each thread has a different random generator.
    // TODO: Set VT terminal when compiling on windows
    Config cfg;
    cfg.samples_per_pixel = 100;
    cfg.image_width = 400;
    cfg.image_height = cfg.image_width * (9.0 / 16.0);
    cfg.filename = "render.png";
    MovableCamera cam(cfg);
    Scene scene;
    cam.debug_info(std::cout);
    image rendered_img;

    // A multi threaded render
    int number_of_threads = std::max(std::thread::hardware_concurrency(), (unsigned int)1);
    rendered_img = multi_threaded_render(cfg, cam, scene, number_of_threads);
    gammacorrect(rendered_img, cfg.gamma);
    std::cout << "Writing to disk....." << std::endl;
    write_to_file(cfg.filename, rendered_img);

    // A single threaded render
    // cfg.filename = "output2-single.png";
    // // cfg.samples_per_pixel *= number_of_threads;
    // rendered_img = Renderer(cfg).render(cam, scene, true);
    // gammacorrect(rendered_img, cfg.gamma);
    // std::cout << "Writing to disk....." << std::endl;
    // write_to_file(cfg.filename, rendered_img);
    return 0;
}
