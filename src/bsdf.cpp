#include "bsdf.h"

BSDFSample BSDF::sample() {
    BSDFSample res;
    res.direction = RNG::sample_hemisphere(res.pdf);
    res.attenuation = albedo / M_PI;
    return res;
}
