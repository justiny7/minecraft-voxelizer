#pragma once

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <ostream>
#include <vector>

#include "ray.h"
#include "vec3.h"

struct BBox {
    BBox() : min(FLT_MAX), max(-FLT_MAX) {}

    explicit BBox(Vec3 min, Vec3 max) : min(min), max(max) {}

    BBox(const BBox&) = default;
    BBox& operator=(const BBox&) = default;
    ~BBox() = default;

    inline Vec3 center() const {
        return (min + max) * 0.5f;
    }

    inline bool empty() const {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    void enclose(Vec3 point);
    bool hit(const Ray &ray, float &t_isect) const; // Ray-BBox intersection

    Vec3 min, max;
};

inline std::ostream& operator<<(std::ostream& out, BBox b) {
    out << "BBox{" << b.min << "," << b.max << "}";
    return out;
}

