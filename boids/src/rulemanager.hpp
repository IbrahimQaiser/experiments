#pragma once
#include "boid.hpp"
#include "config.hpp"
#include <vector>

typedef void (*boid_rule_fptr)(Boid &b);

// This type of rule operates on all neighbours of a boid
// proces_neighbour is called for every boid in the fov of a boid
// and apply is used to apply the steering the to the boid
class NeighbourRule
{
  public:
    virtual void init() = 0;

    virtual void process_neighbour(Boid &current, Boid &neighbour, Vector2 displacment,
                                   float length_sqr) = 0;

    virtual void apply(Boid &boid) = 0;

    virtual ~NeighbourRule() {}
};

class RuleManager
{
    std::vector<int> boid_ids;
    std::vector<boid_rule_fptr> boid_rules;
    std::vector<NeighbourRule *> neighbour_rules;

  public:
    void register_for_every_boid(boid_rule_fptr fp) { boid_rules.push_back(fp); }

    void register_neighbour_rule(NeighbourRule *rule) { neighbour_rules.push_back(rule); }

    void execute(std::vector<Boid> &boids)
    {
        Vector2 displacement = {0, 0};
        float length_sqr = 0;
        for (auto &boid : boids)
        {
            // Execute rules which operate only on a boid
            for (auto &rule : boid_rules)
            {
                rule(boid);
            }

            // Execute rules which depend on nearby boids
            for (auto &rule : neighbour_rules)
                rule->init();

            for (auto &other_boid : boids)
            {
                if (&boid != &other_boid &&
                    boid.is_in_fov(other_boid.position, displacement, length_sqr))
                {
                    for (auto &rule : neighbour_rules)
                        rule->process_neighbour(boid, other_boid, displacement, length_sqr);
                }
            }
            for (auto &rule : neighbour_rules)
                rule->apply(boid);
        }
    }
};