#pragma once

#include <iostream>
#include <cmath>
#include <math.h>
#include "voxelgrid.h"
#include "rand.h"
#include "image.h"

struct Camera {
    Camera(Vec3 position, float yaw, float pitch, int height_, int width_);
    ~Camera() = default;

    int height, width;
    int samples_per_pixel;
    int max_depth = 2;
    int light_samples = 8;
    float aspect_ratio;
    float vert_fov;
    float samples_per_pixel_div;

    Vec3 position, looking_at;
    Vec3 pixel00_loc;
    Vec3 pixel_delta_u, pixel_delta_v;
    Vec3 u, v, w;

    cv::Mat render(VoxelGrid &scene);
    cv::Mat render_normals(VoxelGrid &scene);
    cv::Mat project(VoxelGrid &scene, const Image &img);

    void initialize();
    Ray generate_ray(int pixel_i, int pixel_j, bool rand = true);
    Spectrum trace_ray(const Ray &ray, VoxelGrid &scene, int depth = 0);
    void camera_settings(int height_ = 534, float aspect_ratio_ = 3420.0 / 2138,
            int samples_per_pixel_ = 96, float vert_fov_ = 70.0, bool flying = true);

};
