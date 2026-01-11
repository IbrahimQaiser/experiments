#pragma once
#include "boid.hpp"
#include "config.hpp"
#include "rulemanager.hpp"
#include <vector>

// Makes sure that the boid stays within the screen
void edge_rules(Boid &boid)
{
    int max_x = Config::get().screen_width;
    int max_y = Config::get().screen_height;
    int buffer = Config::get().edge_dist;
    // Make the boid move out of the screen before changing the position
    if (boid.position.x > (max_x + buffer) && boid.velocity.x > 0)
    {
        boid.position.x = -buffer;
    }
    else if (boid.position.x < -buffer && boid.velocity.x < 0)
    {
        boid.position.x = max_x + buffer;
    }

    if (boid.position.y > (max_y + buffer) && boid.velocity.y > 0)
    {
        boid.position.y = -buffer;
    }
    else if (boid.position.y < -buffer && boid.velocity.y < 0)
    {
        boid.position.y = max_y + buffer;
    }
}

// Reverse the boid's velocity to point towards the screen
void edge_rules_reverse_velocity(Boid &boid)
{
    int max_x = Config::get().screen_width;
    int max_y = Config::get().screen_height;
    int buffer = Config::get().edge_dist;
    if (boid.position.x > (max_x + buffer) && boid.velocity.x > 0)
    {
        boid.velocity.x = -boid.velocity.x;
    }
    else if (boid.position.x < -buffer && boid.velocity.x < 0)
    {
        boid.velocity.x = -boid.velocity.x;
    }

    if (boid.position.y > (max_y + buffer) && boid.velocity.y > 0)
    {
        boid.velocity.y = -boid.velocity.y;
    }
    else if (boid.position.y < -buffer && boid.velocity.y < 0)
    {
        boid.velocity.y = -boid.velocity.y;
    }
}

void check_if_velocity_less_than_min_speed(Boid &boid)
{
    if (Vector2Length(boid.velocity) <= Config::get().min_speed)
    {
        boid.velocity = Vector2Scale(Vector2Normalize(boid.velocity), Config::get().min_speed);
    }
}

class SeparationRule : public NeighbourRule
{
    Vector2 steer;

  public:
    void init() { steer = {0.0, 0.0}; }

    void process_neighbour(Boid &current, Boid &neighbour, Vector2 displacement, float length_sqr)
    {
        auto direction = Vector2Negate(Vector2Normalize(displacement));
        steer = Vector2Add(steer, Vector2Scale(direction, 1.0 / (length_sqr)));
    }

    void apply(Boid &boid)
    {
        steer = Vector2Scale(steer, Config::get().separation_coeff);
        boid.velocity = Vector2Add(boid.velocity, steer);
    }
};

class CohesionRule : public NeighbourRule
{
    Vector2 steer, centroid;
    int n;

  public:
    void init()
    {
        steer = {0.0, 0.0};
        centroid = {0.0, 0.0};
        n = 0;
    }

    void process_neighbour(Boid &current, Boid &neighbour, Vector2 displacement, float length_sqr)
    {
        centroid = Vector2Add(centroid, neighbour.position);
        n++;
    }

    void apply(Boid &boid)
    {
        if (n != 0)
        {
            centroid = Vector2Scale(centroid, 1.0 / n);
            steer = Vector2Normalize(Vector2Subtract(centroid, boid.position));
            steer = Vector2Scale(steer, Config::get().cohesion_coeff);
            boid.velocity = Vector2Add(boid.velocity, steer);
        }
    }
};

class AlignmentRule : public NeighbourRule
{
    Vector2 steer, velocity;
    int n;

  public:
    void init()
    {
        steer = {0.0, 0.0};
        velocity = {0.0, 0.0};
        n = 0;
    }

    void process_neighbour(Boid &current, Boid &neighbour, Vector2 displacement, float length_sqr)
    {
        velocity = Vector2Add(velocity, neighbour.velocity);
        n++;
    }

    void apply(Boid &boid)
    {
        if (n != 0)
        {
            velocity = Vector2Scale(velocity, 1.0 / n);
            steer = Vector2Subtract(velocity, boid.velocity);
            steer = Vector2Scale(steer, Config::get().alignment_coeff);
            boid.velocity = Vector2Add(boid.velocity, steer);
        }
    }
};
