#include "boid.hpp"
#include "config.hpp"
#include "raylib.h"
#include "rulemanager.hpp"
#include "rules.hpp"
#include <iostream>
#include <string>
#include <vector>

/// Generates boids with different colors, sizes and max speeds
/// @return vector of boids
std::vector<Boid> get_random_boids()
{
    std::vector<Boid> boids;
    for (int i = 0; i < Config::get().number_boids; i++)
    {
        float x = GetRandomValue(0, Config::get().screen_width);
        float y = GetRandomValue(0, Config::get().screen_height);

        // Vector2 direction = {static_cast<float>(GetRandomValue(-1000, 1000)),
        //                      static_cast<float>(GetRandomValue(-1000, 1000))};

        // Get vector to center of screen
        Vector2 direction = {static_cast<float>((Config::get().screen_width) / 2.0 - x),
                             static_cast<float>((Config::get().screen_height) / 2.0 - y)};
        direction = Vector2Normalize(direction);

        Vector2 velocity = Vector2Scale(
            direction,
            static_cast<float>(
                GetRandomValue(Config::get().min_speed * 10, Config::get().max_speed * 10) / 10.0));

        Color boid_color = {static_cast<unsigned char>(GetRandomValue(0, 200)),
                            static_cast<unsigned char>(GetRandomValue(0, 200)),
                            static_cast<unsigned char>(GetRandomValue(200, 255)), 255};

        int topSpeed = GetRandomValue(Config::get().min_speed, Config::get().max_speed);


        if (i == 0)
        {
            boids.push_back(BoidBuilder()
                                .position({x, y})
                                .color(boid_color)
                                .topSpeed(topSpeed)
                                .velocity(velocity)
                                .size(16)
                                .build());
        }
        else
        {

            boids.push_back(BoidBuilder()
                                .position({x, y})
                                .color(boid_color)
                                .topSpeed(topSpeed)
                                .velocity(velocity)
                                .build());
        }
    }
    return boids;
}

int main(void)
{
    InitWindow(Config::get().screen_width, Config::get().screen_height, Config::get().title);
    ToggleFullscreen();
    SetTargetFPS(Config::get().FPS);
    char buffer[4096];

    RuleManager manager;
    SeparationRule sep;
    CohesionRule coh;
    AlignmentRule ali;

    NeighbourRule *neighbour_rules[] = {&sep, &coh, &ali};

    manager.register_for_every_boid(edge_rules_reverse_velocity);
    manager.register_for_every_boid(check_if_velocity_less_than_min_speed);
    for (auto &rule : neighbour_rules)
        manager.register_neighbour_rule(rule);

    std::vector<Boid> boids = get_random_boids();

    while (!WindowShouldClose())
    {
        // Get key presses
        if (IsKeyPressed(KEY_F2))
            Config::toggle(Config::get().enable_debug);
        if (IsKeyPressed(KEY_R))
            boids = get_random_boids();

        if (IsKeyPressed(KEY_W))
            Config::get().separation_coeff += 50;
        if (IsKeyPressed(KEY_S))
            Config::get().separation_coeff -= 50;

        if (IsKeyPressed(KEY_K))
            Config::get().cohesion_coeff += 0.5;
        if (IsKeyPressed(KEY_J))
            Config::get().cohesion_coeff -= 0.5;

        if (IsKeyPressed(KEY_UP))
            Config::get().alignment_coeff += 0.1;
        if (IsKeyPressed(KEY_DOWN))
            Config::get().alignment_coeff -= 0.1;

        if (IsKeyPressed(KEY_H))
            Config::get().fov_radius += 5;
        if (IsKeyPressed(KEY_L))
            Config::get().fov_radius -= 5;

        if (IsKeyPressed(KEY_U))
            for (auto &boid : boids)
                boid.size -= 0.5;
        if (IsKeyPressed(KEY_I))
            for (auto &boid : boids)
                boid.size += 0.5;

        // Update position, velocity, etc
        float dt = GetFrameTime();

        // Apply rules
        manager.execute(boids);

        // Update velocity and position of boids after applying forces
        for (auto &boid : boids)
        {
            boid.update(dt);
        }

        // Draw the shapes to the screen
        BeginDrawing();
        ClearBackground(RAYWHITE);
        // boids[0].detailed_debug(boids);
        for (const auto &boid : boids)
        {
            boid.draw();
        }
        auto fps = std::to_string(GetFPS());
        snprintf(buffer, 4096, "S: %.02f C: %.02f A: %.02f F: %.02f",
                 Config::get().separation_coeff, Config::get().cohesion_coeff,
                 Config::get().alignment_coeff, Config::get().fov_radius);
        DrawText(fps.c_str(), 10, 10, 40, RED);
        DrawText(buffer, 10, 50, 40, GREEN);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
