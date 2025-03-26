#include <iostream>
#include "camera.h"
#include "voxelgrid.h"
#include "image.h"
#include "voxelizer.h"
#include "parser.h"

int main() {
    VoxelGrid scene0(10, 10, 10);
    {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                scene0.at(i, 0, j).occupancy = 1.0;
            }
        }

        for (int x = 4; x <= 5; x++) {
            for (int y = 1; y <= 5; y++) {
                for (int z = 4; z <= 5; z++) {
                    scene0.at(x, y, z).occupancy = 1.0;
                }
            }
        }

        for (int y : {2, 4}) {
            for (int x : {4, 5}) {
                for (int z : {3, 6}) {
                    scene0.at(x, y, z).occupancy = 1.0;
                    scene0.at(z, y, x).occupancy = 1.0;
                }
            }
        }

        for (int x : {4, 5}) {
            for (int z : {2, 7}) {
                scene0.at(x, 3, z).occupancy = 1.0;
                scene0.at(z, 3, x).occupancy = 1.0;
            }
        }
        for (int x : {3, 6}) {
            for (int z : {3, 6}) {
                scene0.at(x, 3, z).occupancy = 1.0;
            }
        }

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                for (int k = 0; k < 10; k++) {
                    if (scene0.at(i, j, k).occupancy != 1.0) {
                        scene0.remove_block(scene0.at(i, j, k));
                    }
                }
            }
        }
    }
    /*
    VoxelGrid scene1(10, 10, 10);
    {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                scene1.at(i, 0, j).occupancy = 1.0;
            }
        }

        for (int x = 4; x <= 5; x++) {
            for (int z = 4; z <= 5; z++) {
                for (int y : {1, 6}) {
                    scene1.at(x, y, z).occupancy = 1.0;
                }
            }
        }

        for (int x = 4; x <= 5; x++) {
            for (int y = 3; y <= 4; y++) {
                for (int z : {2, 7}) {
                    scene1.at(x, y, z).occupancy = 1.0;
                }
            }
        }

        for (int z = 4; z <= 5; z++) {
            for (int y = 3; y <= 4; y++) {
                for (int x : {2, 7}) {
                    scene1.at(x, y, z).occupancy = 1.0;
                }
            }
        }

        for (int x = 3; x < 7; x++) {
            for (int z = 3; z < 7; z++) {
                for (int y = 2; y < 6; y++) {
                    scene1.at(x, y, z).occupancy = 1.0;
                }
            }
        }
    }

    VoxelGrid scene2(10, 10, 10);
    {
        scene2.at(3, 4, 3).occupancy = 1.0;
    }

    VoxelGrid scene3(10, 10, 10);
    {
        scene3.at(0, 0, 4).occupancy = 1.0;
        scene3.at(0, 1, 4).occupancy = 1.0;
        scene3.at(4, 0, 0).occupancy = 1.0;
        scene3.at(4, 1, 0).occupancy = 1.0;
        scene3.at(3, 0, 1).occupancy = 1.0;
        scene3.at(4, 0, 1).occupancy = 1.0;
        scene3.at(1, 0, 3).occupancy = 1.0;
        scene3.at(2, 0, 3).occupancy = 1.0;
        scene3.at(2, 0, 3).occupancy = 1.0;
        scene3.at(5, 0, 4).occupancy = 1.0;
    }

    VoxelGrid scene4(10, 10, 10);
    {
        scene4.at(2, 0, 2).occupancy = 1.0;
    }

    float yaw = (-42.1 * M_PI / 180), pitch = (-21.2 * M_PI / 180);
    // float pitch = (27.0 * M_PI / 180), yaw = (-24.4 * M_PI / 180);
    // float pitch = M_PI / 2.01, yaw = -M_PI / 4;

    // Vec3 pos(0.968f, 1.44625f + 1.62f, -1.253f);
    Vec3 pos(-3.597f, -0.97398f + 1.62f, -2.923f);

    Image img("view9.png");
    Camera c(pos, yaw, pitch);

    // c.render(scene1);
    c.project(scene1, img);

    // Video v("video.mov", 20);
    */
    // Voxelizer v("video2.mov", 10, 10, 10);

    /*
    for (int i = 3; i < 18; i++) {
        Image img("image_" + std::to_string(i) + ".png");
        Parser p(3);
        p.set_image(img);

        std::cout << p.read(6, 433);
        std::cout << p.find_row('X') << '\n';
        std::cout << p.find_row('F') << '\n';

        Vec3 pos;
        float yaw, pitch;
        std::cout << p.get_pos_camera(pos, yaw, pitch) << '\n';
        std::cout << pos << " " << yaw << " " << pitch << '\n';
    }

    Image img("view7.png");
    Parser p(2);
    p.set_image(img);
    Vec3 pos;
    float yaw, pitch;
    std::cout << p.get_pos_camera(pos, yaw, pitch) << '\n';
    std::cout << pos << " " << yaw << " " << pitch << '\n';
    */

    // /*
    Image img("images/2025-03-14_23.06.17.png");
    Parser p(3);
    p.set_image(img);

    Vec3 pos;
    float yaw, pitch;
    std::cerr << p.get_pos_camera(pos, yaw, pitch) << '\n';
    std::cerr << pos << " " << yaw << " " << pitch << '\n';

    // VoxelGrid scene(10, 10, 10);
    // scene.remove_block(10, 10, 10);
    // scene.remove_block(0, 10, 0);

    Camera c(pos, yaw, pitch, img.height, img.width);

    // cv::Mat res = c.project(scene0, img);
    // cv::imwrite("akljslfkgajlekrjgla.png", res);
    // cv::Mat res = c.render(scene);
    // cv::imshow("Image", res);
    // cv::waitKey(0);
    // cv::imshow("Image", img.silhouette());
    // cv::waitKey(0);

    // cv::Mat res = img.silhouette();
    // cv::imshow("Image", res);
    // cv::waitKey(0);
    // */

    // /*
    Voxelizer v("images/", 10, 10, 10);

    for (int i = 0; i < 10; i++) {
        std::cout << "Starting step " << i + 1 << '\n';
        int removed = v.step();
        std::cout << "Removed " << removed << " blocks\n";
        if (removed == 0) {
            break;
        }
    }

    for(int i = 0; i < v.image_count; i++) {
        cv::imshow("Image", v.cams[i].project(v.scene, v.images[i]));
        cv::waitKey(0);
        cv::imshow("Image", v.cams[i].render_normals(v.scene));
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
    // */

    return 0;
}
