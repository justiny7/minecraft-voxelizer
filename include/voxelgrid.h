#pragma once

#include <fstream>
#include <stack>
#include <utility>
#include <algorithm>

#include "voxel.h"
#include "hitinfo.h"
#include "light.h"

struct VoxelGrid {
    VoxelGrid() {}
    VoxelGrid(std::string build_dir);
    VoxelGrid(int size_x, int size_y, int size_z);

    VoxelGrid(const VoxelGrid&) = default;
    VoxelGrid& operator=(const VoxelGrid&) = default;

    inline int get_idx(size_t x, size_t y, size_t z) {
        assert(x < size_x && y < size_y && z < size_z);
        return x * (size_y * size_z) + y * size_z + z;
    }
    inline void set_lights() {
        lights.clear();

        /*
        // Single white light with corners at 1/3 and 2/3 of x and z axes, 3 units above max y
        Vec3 min(1.0f * size_x / 3, size_y + 3.0f, 1.0f * size_z / 3);
        Vec3 max(2.0f * size_x / 3, size_y + 3.01f, 2.0f * size_z / 3);
        lights.emplace_back(BBox(min, max), Vec3(50, 50, 50));
        */

        // Blue and red square lights in the center
        // Vec3 min(1.0f * size_x / 3, size_y + 3.0f, 1.0f * size_z / 3);
        Vec3 min(0.0f, size_y + 3.0f, 0.0f);
        Vec3 max(1.0f * size_x / 2, size_y + 3.01f, 1.0f * size_z / 2);
        lights.emplace_back(BBox(min, max), Spectrum(Vec3(255.0f, 105.0f, 180.0f), 10.0f));
        // lights.emplace_back(BBox(min, max), Vec3(25.5f, 10.5f, 18.0f));

        Vec3 min2(1.0f * size_x / 2, size_y + 3.0f, 1.0f * size_z / 2);
        Vec3 max2(1.0f * size_x, size_y + 3.01f, 1.0f * size_z);
        // Vec3 max2(2.0f * size_x / 3, size_y + 3.01f, 2.0f * size_z / 3);
        lights.emplace_back(BBox(min2, max2), Spectrum(Vec3(255.0f, 165.0f, 0.0f), 10.0f));
        // lights.emplace_back(BBox(min2, max2), Vec3(25.5f, 16.5f, 0.0f));


        // Weak environment light to illuminate build
        env = HemisphereLight(Spectrum(0.75f));
    }
    
    Voxel& at(size_t x, size_t y, size_t z) { return grid[get_idx(x, y, z)]; }

    void build_octree(Node *node, Vec3 min, Vec3 max);
    void free_octree(Node *node);
    void remove_block(Voxel *v);
    HitInfo hit(const Ray &ray);

    std::vector<Voxel> grid;
    std::vector<Light> lights; // list of rectangular area lights
    HemisphereLight env; // environmental light
    int size_x, size_y, size_z;
    Node *root; // remember to free
};

