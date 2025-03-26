#pragma once

#include "vec3.h"
#include "spectrum.h"
#include <float.h>

struct Ray {
    Ray() = default;
    explicit Ray(Vec3 origin, Vec3 dir) : origin(origin), dir(dir.unit()), throughput(1.0) {}

    inline Vec3 at(float t) const {
        return origin + dir * t;
    }

    Vec3 origin, dir;
    Spectrum throughput;
    mutable float t_min = 0, t_max = FLT_MAX;
};

