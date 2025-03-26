#pragma once

#include "spectrum.h"
#include "rand.h"
#include "bbox.h"

struct LightSample {
    Spectrum radiance;
    Vec3 direction;
    float pdf, distance;
};

struct Light { // Rectangular area light
    Light() {}
    Light(BBox bbox_, Spectrum radiance_);

    LightSample sample(Vec3 from);

    BBox bbox;
    float length, width, height;
    Spectrum radiance;
    Vec3 normal = Vec3(0, -1, 0); // pointing down
};

struct HemisphereLight { // Environmental hemisphere light
    HemisphereLight() {}
    HemisphereLight(Spectrum radiance_) : radiance(radiance_) {}

    LightSample sample();

    Spectrum radiance;
};

