#include "voxelizer.h"

Voxelizer::Voxelizer(const fs::path image_dir, const fs::path load_dir, int gui_scale) {
    scene = VoxelGrid(load_dir);
    if (scene.grid.empty()) {
        return;
    }

    parse_reference_images(image_dir, gui_scale, false);
    loaded = true;
}

Voxelizer::Voxelizer(const fs::path image_dir, int size_x, int size_y, int size_z, int gui_scale) {
    if (!fs::exists(image_dir) || !fs::is_directory(image_dir)) {
        std::cerr << "Error: Could not open image directory (" << image_dir.string() << ")" << std::endl;
        return;
    }

    parse_reference_images(image_dir, gui_scale, true);
    scene = VoxelGrid(size_x, size_y, size_z);
    loaded = false;
}

void Voxelizer::parse_reference_images(const fs::path image_dir, int gui_scale, bool save_silhouettes) {
    cams.clear();

    Parser p(gui_scale);
    for (const auto &entry : fs::directory_iterator(image_dir)) {
        if (fs::is_regular_file(entry)) {
            Image image(entry.path().string());
            if (!image.img.empty()) {
                p.set_image(image);

                Vec3 pos;
                float yaw, pitch;
                if (p.get_pos_camera(pos, yaw, pitch)) {
                    cams.emplace_back(pos, yaw, pitch, image.height, image.width);
                    images.push_back(image);

                    if (save_silhouettes) {
                        cv::Mat silhouette = image.calc_silhouette();
                        Image sil(silhouette);
                        silhouettes.push_back(sil);
                    }
                }

                std::cerr << "Processed " << entry.path().string() << std::endl;
            }
        }
    }

    image_count = images.size();
    build_name = image_dir.stem();
}


int Voxelizer::step() {
    if (loaded) {
        return 0;
    }

    int res = 0;
    for (int i = 0; i < image_count; i++) {
        touched.clear();

        std::cout << "Updating view " << i << "...";
        update_probs(cams[i], silhouettes[i]);
        std::cout << "finished!" << std::endl;

        for (Voxel *v : touched) {
            scene.remove_block(v);
            res++;
        }

    }

    cv::imshow("Update", cams[0].render_normals(scene));
    cv::waitKey(0);

    return res;
}

void Voxelizer::update_probs(Camera &c, Image &img) {
    if (loaded) {
        return;
    }

    std::vector<std::vector<Voxel*>> last_rows(2, std::vector<Voxel*>(c.width, nullptr)); // check for 3x3 intersection with silhouette

    for (int j = 0; j < c.height; j++) {
        std::vector<Voxel*> current_row;

        for (int i = 0; i < c.width; i++) {
            Ray r = c.generate_ray(i, j, false);
            HitInfo h = scene.hit(r);

            if (h.hit_first_bbox) {
                bool ref_hit = img.pixel_color(j, i) == Vec3(255, 255, 255);

                if (h.hit && !ref_hit) {
                    current_row.push_back(h.voxel);

                    if (i > 1 && current_row[i - 1] == h.voxel && current_row[i - 2] == h.voxel &&
                            last_rows[0][i - 2] == h.voxel && last_rows[0][i - 1] == h.voxel && last_rows[0][i] == h.voxel &&
                            last_rows[1][i - 2] == h.voxel && last_rows[1][i - 1] == h.voxel && last_rows[1][i] == h.voxel) {
                        touched.insert(h.voxel);
                    }
                } else {
                    current_row.push_back(nullptr);
                }
            } else {
                current_row.push_back(nullptr);
            }
        }

        last_rows[j % 2].swap(current_row);
    }
}

void Voxelizer::assign_textures() {
    if (loaded) {
        return;
    }

    auto rgb_to_hsv = [](Vec3 col) -> float {
        cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar(col[0], col[1], col[2]));
        cv::Mat hsv;
        cv::cvtColor(rgb, hsv, cv::COLOR_RGB2HSV);
        return hsv.at<cv::Vec3b>(0, 0)[0] * 2.0f;
    };

    // assume textures are in textures/ directory
    std::vector<Image> textures;
    std::vector<float> texture_hues;
    std::vector<Vec3> texture_rgbs;
    if (!fs::exists("textures/") || !fs::is_directory("textures/")) {
        std::cerr << "Error: Could not open texture directory. Please make sure textures/ exists" << std::endl;
        return;
    }

    for (const auto &entry : fs::directory_iterator("textures/")) {
        if (fs::is_regular_file(entry)) {
            Image texture(entry.path().string());
            if (!texture.img.empty()) {
                textures.push_back(texture);

                Vec3 color;
                for (int j = 0; j < texture.height; j++) {
                    for (int i = 0; i < texture.width; i++) {
                        color += texture.pixel_color(j, i);
                    }
                }

                color /= (texture.height * texture.width);
                texture_rgbs.push_back(color);
                texture_hues.push_back(rgb_to_hsv(color));

                std::cout << entry.path().string() << " -> " << color << " " << texture_hues.back() << '\n';
            }
        }
    }

    std::map<Voxel*, float> hues; // assign texture based on average hues of each face
    std::map<Voxel*, int> count;

    auto close_to_edge = [](Vec3 pos) -> bool {
        int c = 0;
        for (int i = 0; i < 3; i++) {
            pos[i] -= std::floor(pos[i]);
            if (pos[i] == 0) {
                c++;
                pos[i] = 0.5;
            }
        }

        return (c > 1) || std::min({pos.x, pos.y, pos.z}) < 0.1 || std::max({pos.x, pos.y, pos.z}) > 0.9;
    };

    for (int t = 0; t < image_count; t++) {
        for (int j = 0; j < cams[t].height; j++) {
            Vec3 row = images[t].pixel_color(j, 0);
            for (int i = 0; i < cams[t].width; i++) {
                Ray r = cams[t].generate_ray(i, j, false);
                HitInfo h = scene.hit(r);
                if (!h.hit || close_to_edge(h.pos)) {
                    continue;
                }

                int face = h.voxel->get_face(h.pos);
                Vec3 color = images[t].pixel_color(j, i);
                Vec3 diff = row - color;
                if (std::max({diff[0], diff[1], diff[2]}) >= 25 || diff.norm() >= 30) { // don't count background
                    h.voxel->ray_hits[face].push_back(color);
                }
            }
        }

        for (Voxel &v : scene.grid) {
            for (int i = 0; i < 6; i++) {
                if (v.ray_hits[i].empty()) {
                    continue;
                }

                Vec3 avg_color;
                for (Vec3 color : v.ray_hits[i]) {
                    avg_color += color;
                }
                avg_color /= v.ray_hits[i].size();

                hues[&v] += rgb_to_hsv(avg_color);
                count[&v]++;

                v.ray_hits[i].clear();
            }
        }
    }

    for (Voxel &v : scene.grid) {
        if (!count[&v]) {
            continue;
        }

        float hue = hues[&v] / count[&v];
        std::cout << v.bbox << " " << hue << '\n';

        int res = 0;
        float min_ = FLT_MAX;
        for (int i = 0; i < textures.size(); i++) {
            if (min_ > abs(texture_hues[i] - hue)) {
                min_ = abs(texture_hues[i] - hue);
                res = i;
            }
        }

        v.set_color(Spectrum::direction(texture_rgbs[res]));
    }

}

void Voxelizer::run() {
    if (loaded) {
        return;
    }

    for (int i = 1; ; i++) {
        std::cout << "Starting step " << i << std::endl;
        int removed = step();
        std::cout << "Removed " << removed << " blocks." << std::endl;

        if (removed == 0) {
            break;
        }
    }

    // std::cout << "Assigning textures..." << std::endl;
    // assign_textures();

    std::cout << "Finished!" << std::endl;
}

void Voxelizer::show_build() {
    for (int i = 0; i < image_count; i++) {
        cv::imshow("Build comparison", cams[i].project(scene, images[i]));
        cv::waitKey(0);
        cv::imshow("Build comparison", cams[i].render_normals(scene));
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
}

void Voxelizer::render_build(std::string render_dir) {
    if (render_dir.back() != '/') {
        render_dir += '/';
    }

    for (int i = 0; i < image_count; i++) {
        std::cout << "Rendering view " << i + 1 << "..." << std::endl;
        cv::Mat render = cams[i].render(scene);
        cv::imwrite(render_dir + "view" + std::to_string(i + 1) + ".png", render);
    }
}

void Voxelizer::save_build(std::string save_dir) {
    if (save_dir.empty()) {
        std::cout << "No save directory given!" << std::endl;
        return;
    }

    if (save_dir.back() != '/') {
        save_dir += '/';
    }

    std::string filename = save_dir + build_name + ".build";
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cout << "Failed to open " << filename << std::endl;
        return;
    }

    std::cout << "Saving build to " << filename << std::endl;

    out << scene.size_x << " " << scene.size_y << " " << scene.size_z << '\n';
    for (const Voxel &v : scene.grid) {
        out << v.visible;
    }

    out.close();
}


