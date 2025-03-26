#include "voxelizer.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    std::string image_dir, save_dir, load_dir, render_dir;
    int x_size, y_size, z_size, gui_scale;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display this help message")
        ("image_dir,d", po::value<std::string>(&image_dir)->required(), "Directory with images of build to reconstruct")
        ("save_dir,s", po::value<std::string>(&save_dir)->default_value(""), "Directory to save builds")
        ("load_dir,l", po::value<std::string>(&load_dir)->default_value(""), "Directory with file to load build")
        ("render_dir,r", po::value<std::string>(&render_dir)->default_value(""), "Directory to save renders")
        ("x_size,x", po::value<int>(&x_size)->default_value(10), "Voxel x size")
        ("y_size,y", po::value<int>(&y_size)->default_value(10), "Voxel y size")
        ("z_size,z", po::value<int>(&z_size)->default_value(10), "Voxel z size")
        ("gui_scale,g", po::value<int>(&gui_scale)->default_value(3), "Minecraft GUI scale");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << desc << "\n";
        return 1;
    }

    Voxelizer *v;
    if (load_dir.empty()) {
        v = new Voxelizer(image_dir, x_size, y_size, z_size, gui_scale);
        v->run();
        v->show_build();
        v->save_build(save_dir);
    } else {
        v = new Voxelizer(image_dir, load_dir, gui_scale);
    }

    if (!render_dir.empty()) {
        v->render_build(render_dir);
    }

    free(v);

    return 0;
}
