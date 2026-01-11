#include "cluster.hpp"
#include <iostream>
#include <random>

struct ClusterSum
{
    long long int r;
    long long int g;
    long long int b;
    long long int a;
    long long int number_of_points;

    ClusterSum() : r(0), g(0), b(0), a(0), number_of_points(0) {}

    ClusterSum &operator+=(Color other)
    {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
    }

    Color get_averaged() const
    {
        Color result;
        if (number_of_points == 0)
            return result;
        result.r = static_cast<unsigned char>(r / number_of_points);
        result.g = static_cast<unsigned char>(g / number_of_points);
        result.b = static_cast<unsigned char>(b / number_of_points);
        result.a = static_cast<unsigned char>(a / number_of_points);
        return result;
    }
};

KMeans::KMeans(int k, int n_iters) : k(k), n_iters(n_iters), centroids(k, Color{}) {}

auto KMeans::fit(const std::vector<Color> &colors) -> int
{
    // 1) Initialize the centroids to random points
    initialize(colors);
    // 2) Assign each point to it's closest centroid
    std::vector<int> clusters(colors.size(), 0);

    for (int i = 1; i <= n_iters; ++i)
    {
        assign_points_to_clusters(colors, clusters);
        // 3) Shift the centroid to the average of items in its cluster
        if (!update_centroids(colors, clusters))
        {
            return i;
        }
    }
    return n_iters;
}

auto KMeans::predict(const std::vector<Color> &colors) -> std::vector<int>
{
    std::vector<int> clusters(colors.size(), 0);
    assign_points_to_clusters(colors, clusters);
    return clusters;
}

auto KMeans::fit_predict(const std::vector<Color> &colors) -> std::vector<int>
{
    // 1) Initialize the centroids to random points
    initialize(colors);
    // 2) Assign each point to it's closest centroid
    std::vector<int> clusters(colors.size(), 0);

    for (int i = 1; i <= n_iters; ++i)
    {
        assign_points_to_clusters(colors, clusters);
        // 3) Shift the centroid to the average of items in its cluster
        if (!update_centroids(colors, clusters))
        {
            return clusters;
        }
    }
    return clusters;
}

auto KMeans::labels() const -> const std::vector<Color> & { return centroids; }

auto KMeans::initialize(const std::vector<Color> &colors) -> void
{
    // TODO: Apply kmeans++
    // For now, just set it to random points
    std::random_device device;
    auto seed = device();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<size_t> point_dist(0, colors.size() - 1);
    for (int i = 0; i < k; ++i)
    {
        auto color = colors[point_dist(rng)];
        centroids[i].r = color.r;
        centroids[i].g = color.g;
        centroids[i].b = color.b;
        centroids[i].a = color.a;
    }
}

auto KMeans::assign_points_to_clusters(const std::vector<Color> &colors,
                                       std::vector<int> &clusters) -> void
{
    for (size_t i = 0; i < colors.size(); ++i)
    {
        auto color = colors[i];
        auto minimum_distance = std::numeric_limits<int>::max();
        int best_cluster = 0;

        for (int m = 0; m < k; ++m)
        {
            auto new_distance = color.distance_squared(centroids[m]);
            if (new_distance < minimum_distance)
            {
                minimum_distance = new_distance;
                best_cluster = m;
            }
        }
        // Assign this color to it's closest cluster
        clusters[i] = best_cluster;
    }
}

auto KMeans::update_centroids(const std::vector<Color> &colors,
                              const std::vector<int> &clusters) -> bool
{
    std::vector<ClusterSum> sum(k);

    // TODO: Check for overflow
    for (size_t i = 0; i < colors.size(); ++i)
    {
        sum[clusters[i]] += colors[i];
        sum[clusters[i]].number_of_points++;
    }

    int number_of_centroids_changed = 0;

    for (auto i = 0; i < k; ++i)
    {
        if (sum[i].number_of_points == 0)
            continue;
        auto previous_centroid = centroids[i];
        centroids[i] = sum[i].get_averaged();

        if (centroids[i] != previous_centroid)
        {
            ++number_of_centroids_changed;
        }
    }
    return number_of_centroids_changed != 0;
}

auto KMeans::from_palette(const std::vector<Color> &palette) -> KMeans
{
    KMeans kmeans(palette.size());
    kmeans.centroids = palette;
    return kmeans;
}