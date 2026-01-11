// Homepage: https://github.com/ananthvk/cpp-raytracer
#include "raytracer.hpp"
#include "progressbar.hpp"
#include <functional>
#include <iostream>
#include <thread>

Renderer::Renderer(const Config &config) : config(config) {}

void Renderer::set_config(const Config cfg) { config = cfg; }

Config Renderer::get_config() const { return config; }

image Renderer::render(const Camera &cam, const Scene &scene, bool show_progress) const
{
    ProgressBar progress_bar(config.image_height, config.progressbar_width, true);
    if (show_progress)
    {
        progress_bar.hide_cursor(std::cout);
        progress_bar.display(std::cout);
    }
    image img(config.image_height, image_row(config.image_width, color()));
    try
    {
        // For each pixel in the image, generate a ray from the camera,
        // then get the color of the ray from the scene.
        // Perform this operation a number of times to sample average the color value for a
        // particular pixel
        if (config.samples_per_pixel <= 0)
            return img;

        for (int i = 0; i < config.image_height; ++i)
        {
            for (int j = 0; j < config.image_width; ++j)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < config.samples_per_pixel; ++sample)
                {
                    auto ray = cam.get_ray(i, j, config.samples_per_pixel > 1);
                    pixel_color += scene.color_at(ray, config.recursion_limit);
                }
                pixel_color /= config.samples_per_pixel;
                // Apply gamma correction at the time of saving
                img[i][j] = pixel_color;
            }
            if (show_progress)
            {
                progress_bar.tick();
                progress_bar.display(std::cout);
            }
        }
        if (show_progress)
        {
            progress_bar.show_cursor(std::cout);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: Error while rendering!" << std::endl;
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    return img;
}

void render(const Renderer &renderer, const Camera &camera, const Scene &scene, image *im)
{
    if (im != nullptr)
    {
        *im = renderer.render(camera, scene, false);
    }
}

image multi_threaded_render(const Config &cfg, const Camera &cam, const Scene &scene,
                            int number_of_threads)
{
    std::cout << "Using " << number_of_threads << " threads" << std::endl;
    auto def_img = image(cfg.image_height, image_row(cfg.image_width, color()));

    // Uses a lot of memory, for each thread a new image is created
    // try optimizing it.
    std::vector<std::thread> threads(number_of_threads);
    std::vector<image> images(number_of_threads, def_img);
    std::vector<Renderer> renderers;
    auto base_spp = cfg.samples_per_pixel / number_of_threads;
    int rem = cfg.samples_per_pixel % number_of_threads;
    // sample per pixel for each of the threads
    std::vector<int> spps(number_of_threads, base_spp);
    int i = 0;
    while (i < rem)
    {
        spps[i]++;
        ++i;
    }
    Config local_cfg = cfg;

    // Create the renderers
    std::cout << "Using SPP: ";
    for (int i = 0; i < number_of_threads; ++i)
    {
        local_cfg.samples_per_pixel = spps[i];
        std::cout << local_cfg.samples_per_pixel << " ";
        renderers.emplace_back(Renderer(local_cfg));
    }
    std::cout << std::endl;

    for (int i = 0; i < number_of_threads; ++i)
    {
        std::cout << "Running thread " << i + 1 << std::endl;
        threads[i] =
            std::thread(render, std::ref(renderers[i]), std::ref(cam), std::ref(scene), &images[i]);
    }

    // Wait for all threads to finish
    for (auto &t : threads)
    {
        t.join();
    }
    // Count number of threads with samples per pixels > 0
    int nonzero = 0;
    for (const auto &k : spps)
    {
        if (k > 0)
        {
            nonzero++;
        }
    }
    std::cout << "All threads finished" << std::endl;
    image rendered_img(cfg.image_height, image_row(cfg.image_width, color()));
    for (int i = 0; i < cfg.image_height; ++i)
    {
        for (int j = 0; j < cfg.image_width; ++j)
        {
            for (int k = 0; k < number_of_threads; ++k)
            {
                rendered_img[i][j] += images[k][i][j] / (double)nonzero;
            }
        }
    }
    return rendered_img;
}
