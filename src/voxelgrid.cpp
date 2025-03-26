#include "voxelgrid.h"

VoxelGrid::VoxelGrid(std::string build_dir) {
    std::ifstream in(build_dir);
    if (!in.is_open() || !(build_dir.size() >= 6 && build_dir.substr(build_dir.size() - 6) == ".build")) {
        std::cout << "Cannot load build!" << std::endl;
        return;
    }

    std::cout << "Loading build from " << build_dir << std::endl;

    in >> size_x >> size_y >> size_z;

    grid.resize(size_x * size_y * size_z);
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                int idx = get_idx(x, y, z);
                grid[idx].bbox.enclose(Vec3(x, y, z));
                grid[idx].bbox.enclose(Vec3(x + 1, y + 1, z + 1));
            }
        }
    }

    root = new Node();
    build_octree(root, Vec3(0, 0, 0), Vec3(size_x, size_y, size_z));

    std::string visibilities;
    in >> visibilities;

    int p = 0;
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                if (visibilities[p++] == '0') {
                    remove_block(&at(x, y, z));
                }
            }
        }
    }

    set_lights();
}

VoxelGrid::VoxelGrid(int size_x, int size_y, int size_z) : size_x(size_x), size_y(size_y), size_z(size_z) {
    grid.resize(size_x * size_y * size_z);

    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                int idx = get_idx(x, y, z);
                grid[idx].bbox.enclose(Vec3(x, y, z));
                grid[idx].bbox.enclose(Vec3(x + 1, y + 1, z + 1));
            }
        }
    }

    root = new Node();
    build_octree(root, Vec3(0, 0, 0), Vec3(size_x, size_y, size_z));

    set_lights();
}

void VoxelGrid::build_octree(Node *node, Vec3 min, Vec3 max) {
    node->bbox = BBox(min, max);

    Vec3 d = max - min;
    node->size = d[0] * d[1] * d[2];
    if (node->size == 1) {
        node->is_leaf = true;
        at(min[0], min[1], min[2]).node = node;
        return;
    }

    Vec3 mid = node->bbox.center();
    std::vector<int> bounds[3];
    for (int i = 0; i < 3; i++) {
        bounds[i].push_back(min[i]);
        if (d[i] != 1) {
            bounds[i].push_back((int)(mid[i] + 0.5));
        }
        bounds[i].push_back(max[i]);
    }

    for (int x = 0; x < bounds[0].size() - 1; x++) {
        for (int y = 0; y < bounds[1].size() - 1; y++) {
            for (int z = 0; z < bounds[2].size() - 1; z++) {
                int idx = (x << 2) + (y << 1) + z;
                node->children[idx] = new Node();
                node->children[idx]->parent = node;

                Vec3 new_min(bounds[0][x], bounds[1][y], bounds[2][z]);
                Vec3 new_max(bounds[0][x + 1], bounds[1][y + 1], bounds[2][z + 1]);
                build_octree(node->children[idx], new_min, new_max);
            }
        }
    }
}

void VoxelGrid::free_octree(Node *node) {
    if (node != nullptr) {
        for (int i = 0; i < 8; i++) {
            free_octree(node->children[i]);
        }
        delete node;
    }
}

void VoxelGrid::remove_block(Voxel *v) {
    v->visible = false;

    Node *node = v->node;
    while (node != nullptr) {
        node->size--;
        node = node->parent;
    }
}

HitInfo VoxelGrid::hit(const Ray &ray) {
    HitInfo ret;

    std::stack<std::pair<Node*, float>> st;
    float initial_hit;
    if (root->empty() || !root->bbox.hit(ray, initial_hit)) {
        return ret;
    }

    ret.hit_first_bbox = true;
    st.emplace(root, initial_hit);
    while (!st.empty()) {
        auto [node, time] = st.top();
        st.pop();

        if (time > ray.t_max) {
            continue;
        }

        if (node->is_leaf) {
            if (!ret.hit || ret.time > time) {
                ret.hit = true;
                ret.time = time;
                ret.pos = ray.at(time);

                BBox b = node->bbox;
                ret.voxel = &(at(b.min[0], b.min[1], b.min[2]));

                ray.t_max = time;
            }
        } else {
            std::vector<std::pair<Node*, float>> v;
            for (int i = 0; i < 8; i++) {
                float t;
                if (node->children[i] != nullptr && !node->children[i]->empty() &&
                        node->children[i]->bbox.hit(ray, t)) {
                    v.emplace_back(node->children[i], t);
                }
            }

            std::sort(v.begin(), v.end(), [&](std::pair<Node*, float> a, std::pair<Node*, float> b) {
                return a.second > b.second;
            });

            for (auto p : v) {
                st.push(p);
            }
        }
    }

    return ret;
}

