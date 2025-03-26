#pragma once

#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include "rand.h"
#include "vec3.h"

struct Image {
    Image(const cv::Mat &img_);
    Image(const std::string &path);

    Image() = default;
    ~Image() = default;

    inline Vec3 pixel_color(int r, int c) const {
        assert(r < img.rows && c < img.cols);
        cv::Vec3b val = img.at<cv::Vec3b>(r, c);
        return Vec3((int)val[2], (int)val[1], (int)val[0]);
    }

    cv::Mat calc_silhouette();

    cv::Mat img;
    int width, height;
};

