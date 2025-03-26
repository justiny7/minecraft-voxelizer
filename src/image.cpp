#include "image.h"

Image::Image(const cv::Mat &img_) {
    img = img_.clone();
    width = img_.cols;
    height = img_.rows;
}

Image::Image(const std::string &path) {
    img = cv::imread(path);
    if (img.empty()) {
        std::cerr << "Error: Could not read image at " << path << std::endl;
        return;
    }

    int crop = 0; // crop black space across top of image
    while (crop < img.rows) {
        bool f = 1;
        for (int i = 0; i < img.cols; i++) {
            f &= pixel_color(crop, i) == Vec3(0, 0, 0);
        }

        if (!f) {
            break;
        }

        crop++;
    }

    img = img(cv::Rect(0, crop, img.cols, img.rows - crop));

    width = img.cols, height = img.rows;
}

cv::Mat Image::calc_silhouette() {
    std::vector<std::vector<bool>> occupied(height, std::vector<bool>(width));
    for (int j = 0; j < height; j++) {
        Vec3 row = pixel_color(j, 0);
        for (int i = 0; i < width; i++) {
            Vec3 diff = row - pixel_color(j, i);
            if (std::max({diff[0], diff[1], diff[2]}) >= 25 || diff.norm() >= 30) {
                occupied[j][i] = true;
            }
        }
    }

    std::vector<unsigned char> vals;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            bool f = occupied[j][i];
            for (int dj : {-1, -1, -1, 0, 1, 1, 1, 0}) {
                for (int di : {-1, 0, 1, 1, 1, 0, -1, -1}) {
                    int nj = j + dj, ni = i + ni;
                    if (nj >= 0 && nj < height && ni >= 0 && ni < width) {
                        f |= occupied[nj][ni];
                    }
                }
            }

            if (f) {
                vals.push_back(255);
                vals.push_back(255);
                vals.push_back(255);
            } else {
                vals.push_back(0);
                vals.push_back(0);
                vals.push_back(0);
            }
        }
    }

    cv::Mat result(height, width, CV_8UC3);
    std::memcpy(result.data, vals.data(), vals.size() * sizeof(unsigned char));
    return result;
}
