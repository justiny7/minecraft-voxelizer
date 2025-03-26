#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <fstream>

#include "image.h"
#include "bbox.h"
#include "bsdf.h"

struct Node { // octree nodes
    Node() : size(0), children(), is_leaf(false), parent(nullptr) {}

    BBox bbox;
    int size; // number of occupied voxels
    Node *children[8];
    Node *parent;
    bool is_leaf;

    bool empty() { return size == 0; }
};

struct Voxel {
    Voxel() : visible(true), node(nullptr) {}

    Voxel(const Voxel&) = default;
    Voxel& operator=(const Voxel&) = default;
    ~Voxel() = default;


    Vec3 get_normal(const Vec3 &hit) {
        Vec3 res;
        for (int i = 0; i < 3; i++) {
            if (abs(hit[i] - bbox.min[i]) < 1e-5) {
                res[i] = -1;
                break;
            } else if (abs(hit[i] - bbox.max[i]) < 1e-5) {
                res[i] = 1;
                break;
            }
        }

        return res;
    }
    int get_face(const Vec3 &hit) {
        Vec3 normal = get_normal(hit);
        if (normal.x == -1) return 0;
        if (normal.x == 1) return 1;
        if (normal.y == -1) return 2;
        if (normal.y == 1) return 3;
        if (normal.z == -1) return 4;
        return 5;
    }
    void set_color(Spectrum color) { material = BSDF(color); }

    bool visible;
    BBox bbox;
    std::vector<Vec3> ray_hits[6];
    Node *node;
    BSDF material;
};

