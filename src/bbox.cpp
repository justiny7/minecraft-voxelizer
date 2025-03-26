#include "bbox.h"

void BBox::enclose(Vec3 point) {
    min = hmin(min, point);
    max = hmax(max, point);
}

bool BBox::hit(const Ray &ray, float &t_isect) const {
    float t_min = ray.t_min, t_max = ray.t_max;

    Vec3 o = ray.origin, d = ray.dir;
    for (int face = 0; face < 3; face++) {
        if (abs(d[face]) < 1e-5) {
            if (o[face] < min[face] || o[face] > max[face]) {
                return false;
            }
            continue;
        }

        float a = 1.0f / d[face];
        float b = -o[face] * a;
        float t1 = a * min[face] + b, t2 = a * max[face] + b;
        if (t1 > t2) {
            std::swap(t1, t2);
        }

        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);

        if (t_min > t_max) {
            return false;
        }
    }

    t_isect = t_min;
    return true;
}
