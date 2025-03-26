#include "camera.h"
#include "rand.h"

Camera::Camera(Vec3 position, float yaw, float pitch, int height_, int width_) : position(position) {
    yaw = yaw * M_PI / 180, pitch = pitch * M_PI / 180;
    Vec3 dir(-std::cos(pitch) * std::sin(yaw), -std::sin(pitch), std::cos(pitch) * std::cos(yaw));
    looking_at = position + dir;

    camera_settings(height_, 1.0 * width_ / height_);
    initialize();
}

void Camera::camera_settings(int height_, float aspect_ratio_,
        int samples_per_pixel_, float vert_fov_, bool flying) {
    height = height_;
    aspect_ratio = aspect_ratio_;
    samples_per_pixel = samples_per_pixel_;
    vert_fov = vert_fov_ * (flying ? 1.1 : 1.0);
}

cv::Mat Camera::render(VoxelGrid &scene) {
    height = 500;
    initialize();

    std::vector<unsigned char> pixels;

    for (int j = 0; j < height; j++) {
        if (j > 0 && j % 100 == 0) {
            std::cout << "Rendered " << j << " out of " << height << " lines" << std::endl;
        }

        for (int i = 0; i < width; i++) {
            Spectrum color;
            for (int s = 0; s < samples_per_pixel; s++) {
                Ray r = generate_ray(i, j, (samples_per_pixel > 1));
                Spectrum sample = trace_ray(r, scene);

                float rr = 1.0 - std::exp(-sample.r),
                      gr = 1.0 - std::exp(-sample.g),
                      br = 1.0 - std::exp(-sample.b);
                color += Spectrum(rr, gr, br).srgb();
                // color += trace_ray(r, scene).apply_reinhard().srgb();
            }

            color = color * samples_per_pixel_div * 255.0;
            pixels.push_back(color.b);
            pixels.push_back(color.g);
            pixels.push_back(color.r);
        }
    }

    cv::Mat result(height, width, CV_8UC3);
    std::memcpy(result.data, pixels.data(), pixels.size() * sizeof(unsigned char));
    return result;
}

cv::Mat Camera::render_normals(VoxelGrid &scene) {
    std::vector<unsigned char> pixels;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Vec3 hits;
            Ray r = generate_ray(i, j, (samples_per_pixel > 1));
            HitInfo h = scene.hit(r);

            if (h.hit) {
                Voxel &vox = *h.voxel;
                Vec3 normal = vox.get_normal(h.pos);

                hits = normal * 100 + Vec3(150, 150, 150);
            }

            pixels.push_back(hits[0]);
            pixels.push_back(hits[1]);
            pixels.push_back(hits[2]);
        }
    }

    cv::Mat result(height, width, CV_8UC3);
    std::memcpy(result.data, pixels.data(), pixels.size() * sizeof(unsigned char));
    return result;
}

cv::Mat Camera::project(VoxelGrid &scene, const Image &img) {
    std::vector<unsigned char> pixels;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int img_i = (1.0 * i * img.width) / width,
                img_j = (1.0 * j * img.height) / height;
            Vec3 color = img.pixel_color(img_j, img_i);
            
            Ray r = generate_ray(i, j, false);
            HitInfo h = scene.hit(r);

            if (h.hit) {
                pixels.push_back((unsigned char)color[2]);
                pixels.push_back((unsigned char)color[1]);
                pixels.push_back((unsigned char)color[0]);

            } else {
                Vec3 row = img.pixel_color(img_j, 0); // Approximate background by first pixel in row
                pixels.push_back((unsigned char)row[2]);
                pixels.push_back((unsigned char)row[1]);
                pixels.push_back((unsigned char)row[0]);
            }
        }
    }

    cv::Mat result(height, width, CV_8UC3);
    std::memcpy(result.data, pixels.data(), pixels.size() * sizeof(unsigned char));
    return result;
}

void Camera::initialize() {
    width = (int)(height * aspect_ratio);

    float vh = 2.0 * std::tan((vert_fov * M_PI / 180.0) / 2);
    float vw = vh * aspect_ratio;

    w = (position - looking_at).unit();
    u = cross(Vec3(0, 1, 0), w).unit();
    v = cross(w, u);

    Vec3 vu = vw * u, vv = vh * -v;

    pixel_delta_u = vu / width, pixel_delta_v = vv / height;

    pixel00_loc = (position - w - 0.5 * (vu + vv)) + 0.5 * (pixel_delta_u + pixel_delta_v);

    samples_per_pixel_div = 1.0 / samples_per_pixel;
}

Ray Camera::generate_ray(int pixel_i, int pixel_j, bool rand) {
    float offset_i = 0, offset_j = 0;
    if (rand) {
        offset_i = RNG::unit() - 0.5, offset_j = RNG::unit() - 0.5;
    }

    Vec3 sample = pixel00_loc + (pixel_i + offset_i) * pixel_delta_u + (pixel_j + offset_j) * pixel_delta_v;
    return Ray(position, sample - position);
}

Spectrum Camera::trace_ray(const Ray &ray, VoxelGrid &scene, int depth) {
    HitInfo h = scene.hit(ray);
    if (!h.hit) {
        Spectrum radiance;
        for (Light light : scene.lights) {
            float temp;
            if (light.bbox.hit(ray, temp)) {
                radiance += light.radiance;
            }
        }

        // radiance = 255.0 * (radiance / (1.0f + radiance));
        return radiance;
    }

    auto transform_to_normal = [](Vec3 v, const Vec3 &normal) -> Vec3 {
        if (normal.x == 1) {
            return Vec3(v.y, -v.z, v.x);
        } else if (normal.x == -1) {
            return Vec3(-v.y, v.z, -v.x);
        } else if (normal.y == -1) {
            return Vec3(-v.x, -v.y, v.z);
        } else if (normal.z == 1) {
            return Vec3(v.x, -v.z, v.y);
        } else if (normal.z == -1) {
            return Vec3(v.x, v.z, -v.y);
        }

        return v;
    };

    Vec3 normal = h.voxel->get_normal(h.pos);
    Spectrum radiance;

    BSDFSample bs = h.voxel->material.sample();
    
    // Direct lighting
    for (Light light : scene.lights) {
        for (int _ = 0; _ < light_samples; _++) {
            LightSample sample = light.sample(h.pos);

            float cos_theta = dot(sample.direction, normal);
            if (cos_theta <= 0.0f) {
                continue;
            }

            Ray shadow_ray(h.pos, sample.direction);
            shadow_ray.t_min = 1e-5;

            HitInfo shadow_hit = scene.hit(shadow_ray);
            if (!shadow_hit.hit) { // finds light
                float cos_theta_p = dot(-sample.direction, light.normal);

                if (cos_theta_p > 0.0f) {
                    float dw = cos_theta_p / (sample.distance * sample.distance);
                    radiance += (cos_theta * dw / (light_samples * sample.pdf)) * sample.radiance * bs.attenuation;
                }
            }
        }
    }

    // Environmental lighting from hemisphere on first bounce
    if (depth == 0) {
        for (int _ = 0; _ < light_samples; _++) {
            LightSample sample = scene.env.sample();

            float cos_theta = dot(sample.direction, normal);
            if (cos_theta <= 0.0f) {
                continue;
            }

            Ray shadow_ray(h.pos, sample.direction);
            shadow_ray.t_min = 1e-5;

            HitInfo shadow_hit = scene.hit(shadow_ray);
            if (!shadow_hit.hit) {
                radiance += (cos_theta / (light_samples * sample.pdf)) * sample.radiance * bs.attenuation;
            }
        }
    }

    // Indirect lighting
    if (depth < max_depth) {
        Ray indirect(h.pos, transform_to_normal(bs.direction, normal));
        indirect.throughput = ray.throughput * bs.attenuation * (bs.direction.y / bs.pdf);
        indirect.t_min = 1e-5;

        // Russian roulette
        float q = 1.0 - indirect.throughput.luma();
        if (RNG::coin_flip(std::max(q, 0.0f))) {
            return radiance;
        }

        indirect.throughput *= 1.0 / (1 - q);

        radiance += indirect.throughput * trace_ray(indirect, scene, depth + 1);
    }

    // radiance = 255.0 * (radiance / (1.0f + radiance));

    return radiance;
}

