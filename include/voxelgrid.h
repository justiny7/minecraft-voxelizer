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

        /*
        // Small blue and red square lights in the center
        Vec3 min(1.0f * size_x / 3, size_y + 3.0f, 1.0f * size_z / 3);
        Vec3 max(1.0f * size_x / 2, size_y + 3.01f, 1.0f * size_z / 2);
        lights.emplace_back(BBox(min, max), Spectrum(Vec3(255.0f, 0.0f, 0.0f), 40.0f));

        Vec3 min2(1.0f * size_x / 2, size_y + 3.0f, 1.0f * size_z / 2);
        Vec3 max2(2.0f * size_x / 3, size_y + 3.01f, 2.0f * size_z / 3);
        lights.emplace_back(BBox(min2, max2), Spectrum(Vec3(0.0f, 0.0f, 255.0f), 40.0f));
        */

        // Big Microsoft logo lights
        Vec3 min(0.0f, size_y + 3.0f, 0.0f);
        Vec3 max(1.0f * size_x / 2, size_y + 3.01f, 1.0f * size_z / 2);
        lights.emplace_back(BBox(min, max), Spectrum(Vec3(127.0f, 186.0f, 0.0f), 40.0f));

        Vec3 min2(1.0f * size_x / 2, size_y + 3.0f, 0.0f);
        Vec3 max2(1.0f * size_x, size_y + 3.01f, 1.0f * size_z / 2);
        lights.emplace_back(BBox(min2, max2), Spectrum(Vec3(242.0f, 80.0f, 34.0f), 40.0f));

        Vec3 min3(1.0f * size_x / 2, size_y + 3.0f, 1.0f * size_z / 2);
        Vec3 max3(1.0f * size_x, size_y + 3.01f, 1.0f * size_z);
        lights.emplace_back(BBox(min3, max3), Spectrum(Vec3(0.0f, 164.0f, 239.0f), 40.0f));

        Vec3 min4(0.0f, size_y + 3.0f, 1.0f * size_z / 2);
        Vec3 max4(1.0f * size_x / 2, size_y + 3.01f, 1.0f * size_z);
        lights.emplace_back(BBox(min4, max4), Spectrum(Vec3(255.0f, 185.0f, 0.0f), 40.0f));


        // Weak environment light to illuminate build
        env = HemisphereLight(Spectrum(Vec3(255.0f), 0.05f));
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

