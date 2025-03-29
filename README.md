# Minecraft Voxelizer

## Reconstruct Minecraft builds in 3D from screenshots

### How it works
The voxelizer starts off with a rectangular block of voxels, then uses voxel carving to eliminate blocks by comparing the silhouettes of the current grid against the reference images. After each carving step, the program will show a visual of the current state of the grid, waiting for a keypress before beginning the next iteration. Once the grid converges, the program displays the grid from the angle of each reference picture against the actual picture itself. From there, you can load/render builds from different angles with different lighting.

To do so, the program uses a ray tracer inspired by the implementation [here](https://github.com/RayTracing/raytracing.github.io), a parser to extract coordinates, and an octree data structure to speed up ray-voxel intersections. Various class implementations are inspired by [Cardinal3D](https://github.com/stanford-cs248/Cardinal3D), as this is a more developed version of my final CS 248A project at Stanford.

### Results
<p align="center">
    <img src="https://github.com/user-attachments/assets/489b6914-eadd-439c-8ba4-8276838f0f70" width="250">
    <img src="https://github.com/user-attachments/assets/a7d50502-ede5-4077-9e31-9cabd4389aad" width="250">
    <img src="https://github.com/user-attachments/assets/fa34ee03-efcd-41f0-bfd0-a6a67fa424ca" width="250">
</p>
Complex geometric build renders

<p align="center">
    <img src="https://github.com/user-attachments/assets/a0c1cedf-1cda-4bb6-a2bf-1cf904001fe2" width="250"> // VIEW 16
    <img src="https://github.com/user-attachments/assets/365a0dc2-8197-40dd-9b0d-961e334c7905" width="250"> // VIEW 6
    <img src="https://github.com/user-attachments/assets/ba88e612-6871-4d6e-a699-39eadf0325a9" width="250"> // VIEW 7
</p>
Dog build renders

## How to use it

### Build
Ensure you have C++17 or higher, OpenCV, and Boost installed, then:
```bash
mkdir build
cd build
cmake ..
```

### Set up screenshots
1. Create a superflat void world in Minecraft with default texture pack and clear all blocks
2. Set time of day to noon (`/time set noon`) and turn off daylight cycle (`/gamerule doDaylightCycle false`)
3. Navigate to (0, 0, 0) and begin building in the positive X/Y/Z octant, preferably with blocks that are a different color from the background
4. Open F3 debug screen, take screenshots of the build from multiple angles with F2, save screenshots to folder

### Command line interface
| Flag          | Short | Type    | Default | Description                                  |
|--------------|-------|---------|---------|----------------------------------------------|
| `--help`     | `-h`  | N/A     | N/A     | Display this help message                   |
| `--image_dir`| `-d`  | string  | (required) | Directory with images of build to reconstruct |
| `--save_dir` | `-s`  | string  | `""`    | Directory to save builds                    |
| `--load_dir` | `-l`  | string  | `""`    | Directory with file to load build           |
| `--render_dir` | `-r` | string  | `""`    | Directory to save renders                   |
| `--x_size`   | `-x`  | int     | `10`    | Voxel x size                                |
| `--y_size`   | `-y`  | int     | `10`    | Voxel y size                                |
| `--z_size`   | `-z`  | int     | `10`    | Voxel z size                                |
| `--gui_scale`| `-g`  | int     | `3`     | Minecraft GUI scale (Options -> Video settings) |

**image_dir flag is required to extract camera positions for reconstructing/rendering**

Examples:
```bash
# Reconstruct and render build with default 10x10x10 voxel grid and GUI scale of 3
./MinecraftReconstructor -d path_to_screenshots -r folder_to_save_renders

# Reconstruct and save build to 10x15x20 voxel grid and GUI scale of 2
./MinecraftReconstructor -d path_to_screenshots -s folder_to_save_build -y 15 -z 20 -g 2

# Load and render build from camera positions in screenshots
./MinecraftReconstructor -d path_to_screenshots -l path_to_build_file -r folder_to_save_renders
```

### Samples
There are also folders with sample screenshots, as well as their render results. They were created with the following commands in the `/build` folder:
```bash
# Complex: 10x10x10 grid, GUI scale 3
# Dog: 10x13x13 grid, GUI scale 2

# Reconstructing
./MinecraftReconstructor -d ../images/complex -s ../saved_builds
./MinecraftReconstructor -d ../images/dog -s ../saved_builds -y 13 -g 2

# Rendering
./MinecraftReconstructor -d ../images/complex -l ../saved_builds/complex.build -r ../renders/complex
./MinecraftReconstructor -d ../images/dog -l ../saved_builds/dog.build -r ../renders/dog -g 2
```
The different lightings used to render the builds are both in the `set_lights()` function of `include/voxelgrid.h`.

### More customization
If you want to play around with different lightings, you can edit the `set_lights()` function in `include/voxelgrid.h`. If you want to add custom camera positions outside of those extracted from screenshots, you can edit the `Voxelizer` constructors in `src/voxelizer.cpp` - specifically, changing the cameras pushed into the `cams` vector. Feel free to add more command line options by editing `src/main.cpp`!


