#pragma once

#include <ctime>
#include <chrono>
#include <random>
#include <math.h>

#include "vec3.h"

namespace RNG {

    float unit();
    int integer(int a, int b);
    bool coin_flip(float p);

    Vec3 sample_hemisphere_cosine(float &pdf);
    Vec3 sample_hemisphere_uniform(float &pdf);

} // namespace RNG

