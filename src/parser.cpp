#include "parser.h"

Parser::Parser(int scale_) : scale(scale_) {
    pixels['0'] = "01110100011001110101110011000101110";
    pixels['1'] = "00100011000010000100001000010011111";
    pixels['2'] = "01110100010000100110010001000111111";
    pixels['3'] = "01110100010000100110000011000101110";
    pixels['4'] = "00011001010100110001111110000100001";
    pixels['5'] = "11111100001111000001000011000101110";
    pixels['6'] = "00110010001000011110100011000101110";
    pixels['7'] = "11111100010000100010001000010000100";
    pixels['8'] = "01110100011000101110100011000101110";
    pixels['9'] = "01110100011000101111000010001001100";
    pixels['X'] = "10001010100010001010100011000110001";
    pixels['F'] = "11111100001110010000100001000010000";
    pixels['n'] = "00000000001111010001100011000110001";
    pixels['s'] = "00000000000111110000011100000111110";
    pixels['e'] = "00000000000111010001111111000001111";
    pixels['w'] = "00000000001000110001101011010101111";
    pixels['-'] = "00000000000000011111000000000000000";
    pixels['/'] = "00001000100001000100010000100010000";
    pixels[' '] = "00000000000000000000000000000000000";
    pixels['.'] = "00000000000010";
}

bool Parser::get_pos_camera(Vec3 &position, float &yaw, float &pitch) {
    int XYZ_row = find_row('X'), cam_row = find_row('F');
    if (XYZ_row == -1 || cam_row == -1) {
        return false;
    }

    bool res = true;
    res &= get_XYZ(XYZ_row, position);
    res &= get_cam(cam_row, yaw, pitch);

    position.y += 1.62; // offset from base of player
    return res;
}

char Parser::read(int row, int col) {
    // Finds char with minimum sum of weighted standard deviations of in/out groups
    if (row < 0 || col < 0 || row >= img.height || col >= img.width) {
        return '\0';
    }

    char res = '\0';
    float min_score = FLT_MAX;
    for (const auto &[c, p] : pixels) {
        int w = CHAR_WIDTH, h = CHAR_HEIGHT;
        if (c == '.') {
            w = 2;
        }

        if (row + w * scale - 1 >= img.height || col + h * scale - 1 >= img.width) {
            continue;
        }

        float score = 0;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                bool cur = p[i * w + j] == '1';
                for (int s1 = 0; s1 < scale; s1++) {
                    for (int s2 = 0; s2 < scale; s2++) {
                        Vec3 color = img.pixel_color(row + i * scale + s1, col + j * scale + s2);
                        float avg = (color[0] + color[1] + color[2]) / 3,
                              mn = std::min({color[0], color[1], color[2]});
                        score += (avg > 185 && mn > 175) != cur;
                    }
                }
            }
        }

        score /= (w * h);

        if (min_score > score) {
            min_score = score;
            res = c;
        }
    }

    // If score deviates too much, then no character is recognized
    return (min_score < 1.0 ? res : '\0');
}


int Parser::find_row(char starting_char) {
    int start_row = 0, start_col = 2 * scale;
    while (img.pixel_color(start_row, 0) == Vec3(0, 0, 0)) {
        start_row++;
    }
    start_row += (CHAR_HEIGHT + 2) * 10 * scale;

    for (int i = 0; i < SEARCH_LIMIT; i += scale) {
        if (read(start_row + i, start_col) == starting_char) {
            return start_row + i;
        }
    }

    return -1;
}

bool Parser::search_until(int row, int &col, char target) {
    char c = read(row, col);
    for (int i = 0; i < SEARCH_LIMIT && c != target; i++) {
        col += scale;
        c = read(row, col);
    }

    return c == target;
}

void Parser::read_number(int row, int &col, float &num) {
    num = 0;
    bool decimal = false;
    int div = 1, neg = 1;

    char c = read(row, col);
    while (c != '\0') {
        if (c == '-') {
            neg = -1;
            col += (CHAR_WIDTH + 1) * scale;
        } else if (c >= '0' && c <= '9') {
            num = num * 10 + (c - '0');
            col += (CHAR_WIDTH + 1) * scale;
            
            if (decimal) {
                div *= 10;
            }
        } else if (c == '.') {
            if (decimal) {
                break;
            }

            decimal = true;
            col += 2 * scale;
        } else {
            break;
        }

        c = read(row, col);
    }

    num /= div * neg;
}

bool Parser::get_XYZ(int row, Vec3 &position) {
    int col = ((CHAR_WIDTH + 1) * 4 + 2) * scale;

    Vec3 res;
    for (int i = 0; i < 3; i++) {
        read_number(row, col, res[i]);

        if (i < 2) {
            if (!search_until(row, col, '/')) {
                return false;
            }

            col += CHAR_WIDTH * 2 * scale;
        }
    }

    position = res;
    return true;
}

bool Parser::get_cam(int row, float &yaw, float &pitch) {
    char dir = read(row, 40 * scale);

    int col = scale;
    if (dir == 'n') {
        col *= 186;
    } else if (dir == 's') {
        col *= 182;
    } else if (dir == 'e') {
        col *= 176;
    } else if (dir == 'w') {
        col *= 180;
    } else {
        return false;
    }

    read_number(row, col, yaw);

    if (!search_until(row, col, '/')) {
        return false;
    }

    col += CHAR_WIDTH * 2 * scale;
    read_number(row, col, pitch);

    return true;
}

