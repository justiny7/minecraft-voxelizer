#include "light.h"

Light::Light(BBox bbox_, Spectrum radiance_) : bbox(bbox_), radiance(radiance_) {
    Vec3 extent = bbox.max - bbox.min;
    length = extent.x, width = extent.z;
    height = bbox.min.y;
}

LightSample Light::sample(Vec3 from) {
    LightSample res;
    res.radiance = radiance;

    Vec3 random(RNG::unit() * length, 0.0f, RNG::unit() * width);
    Vec3 point = bbox.min + random;
    res.distance = (point - from).norm();
    res.direction = (point - from).unit();
    res.pdf = 1.0 / (length * width);
    return res;
}

LightSample HemisphereLight::sample() {
    LightSample res;
    res.direction = RNG::sample_hemisphere(res.pdf);
    res.radiance = radiance * res.direction.y;
    res.distance = FLT_MAX;
    return res;
}
