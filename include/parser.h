#pragma once

#include "image.h"

#define CHAR_WIDTH 5 // Minecraft debug screen font constants
#define CHAR_HEIGHT 7

#define SEARCH_LIMIT 300

struct Parser {
    Parser(int scale_ = 3);

    inline void set_image(const Image &img_) {
        img = img_;
    }

    bool get_pos_camera(Vec3 &position, float &yaw, float &pitch);
    char read(int row, int col);
    int find_row(char starting_char);
    bool get_XYZ(int row, Vec3 &position);
    bool get_cam(int row, float &yaw, float &pitch);

    void read_number(int row, int &col, float &num);
    bool search_until(int row, int &col, char target);


    std::map<char, std::string> pixels;
    Image img;
    int scale; // GUI scale in Minecraft settings
};


