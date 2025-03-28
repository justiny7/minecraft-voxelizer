#pragma once

#include "spectrum.h"
#include "rand.h"

struct BSDFSample {
    Spectrum attenuation;
    Vec3 direction;
    float pdf;
};

struct BSDF { // Lambertian
    BSDF() : albedo(1.0) {}
    BSDF(Spectrum albedo_) : albedo(albedo_ / 255.0) {}

    BSDFSample sample(); // cosine-weighted hemisphere sampling

    Spectrum albedo;
};

