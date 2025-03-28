#include "rand.h"

namespace RNG {

    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    float unit() {
        std::uniform_real_distribution<float> d(0.0f, 1.0f);
        return d(rng);
    }

    int integer(int a, int b) {
        std::uniform_int_distribution<int> d(a, b);
        return d(rng);
    }

    bool coin_flip(float p) {
        return unit() < p;
    }

    Vec3 sample_hemisphere_cosine(float &pdf) { // cosine-weighted hemisphere sampling
        float Xi1 = RNG::unit();
        float Xi2 = RNG::unit();

        float theta = std::acos(std::sqrt(Xi1));
        float phi = 2.0f * M_PI * Xi2;

        float xs = std::sin(theta) * std::cos(phi);
        float ys = std::cos(theta);
        float zs = std::sin(theta) * std::sin(phi);

        pdf = ys / M_PI;
        return Vec3(xs, ys, zs);
    }

    Vec3 sample_hemisphere_uniform(float &pdf) { // cosine-weighted hemisphere sampling
        float Xi1 = RNG::unit();
        float Xi2 = RNG::unit();

        float theta = std::acos(Xi1);
        float phi = 2.0f * M_PI * Xi2;

        float xs = std::sin(theta) * std::cos(phi);
        float ys = std::cos(theta);
        float zs = std::sin(theta) * std::sin(phi);

        pdf = 1 / (2.0f * M_PI);
        return Vec3(xs, ys, zs);
    }

} // namespace RNG

