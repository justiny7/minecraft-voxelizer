#pragma once

#include "voxel.h"
#include "vec3.h"

struct HitInfo {
    bool hit = false;
    bool hit_first_bbox = false;
    float time = 0;
    Vec3 pos;
    Voxel *voxel = nullptr;
};
