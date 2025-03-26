#pragma once

#include <filesystem>
#include <set>
#include <map>

#include "camera.h"
#include "parser.h"

namespace fs = std::filesystem;

struct Voxelizer {
    Voxelizer(const fs::path image_dir, const fs::path load_dir, int gui_scale = 3);
    Voxelizer(const fs::path image_dir, int size_x, int size_y, int size_z, int gui_scale = 3);
    ~Voxelizer() { scene.free_octree(scene.root); }

    void parse_reference_images(const fs::path image_dir, int gui_scale, bool save_silhouettes);

    int step(); // returns number of blocks removed from this step
    void run();
    void update_probs(Camera &c, Image &img);
    void assign_textures();
    void show_build();
    void render_build(std::string render_dir);
    void save_build(std::string save_dir);

    bool loaded;
    int image_count;
    VoxelGrid scene;
    std::string build_name;
    std::vector<Camera> cams;
    std::vector<Image> images, silhouettes;
    std::set<Voxel*> touched;
};

