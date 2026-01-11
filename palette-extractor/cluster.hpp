#pragma once

#include "image.hpp"
#include <vector>

class Cluster
{
  public:
    virtual ~Cluster() {}

    virtual auto fit(const std::vector<Color> &colors) -> int = 0;
    virtual auto labels() const -> const std::vector<Color> & = 0;
    virtual auto predict(const std::vector<Color> &colors) -> std::vector<int> = 0;
    virtual auto fit_predict(const std::vector<Color> &colors) -> std::vector<int> = 0;
};

class KMeans : public Cluster
{
    int k;
    int n_iters;
    std::vector<Color> centroids;
    auto initialize(const std::vector<Color> &colors) -> void;
    auto assign_points_to_clusters(const std::vector<Color> &colors,
                                   std::vector<int> &clusters) -> void;

    // @return `true` if any of the centroids shifted during the iteration
    auto update_centroids(const std::vector<Color> &colors,
                          const std::vector<int> &clusters) -> bool;

  public:
    KMeans(int k, int n_iters = 100);
    auto fit(const std::vector<Color> &colors) -> int;
    auto labels() const -> const std::vector<Color> &;
    auto predict(const std::vector<Color> &colors) -> std::vector<int>;
    auto fit_predict(const std::vector<Color> &colors) -> std::vector<int>;
    static auto from_palette(const std::vector<Color>& palette) -> KMeans;
};