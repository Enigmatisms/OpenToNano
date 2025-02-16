#include <iostream>
#include <fstream>
#include <stdexcept> // 包含标准异常处理

#include <openvdb/openvdb.h>
#include <nanovdb/util/CreateNanoGrid.h>

void readWorldSpaceBoundingBox(openvdb::GridBase::Ptr grid) {
    const openvdb::math::Transform& transform = grid->transform();

    openvdb::CoordBBox localBBox = grid->evalActiveVoxelBoundingBox();

    auto worldBBox = transform.indexToWorld(localBBox);

    std::cout << "World Space Bounding Box: "
              << "min = " << worldBBox.min() << ", "
              << "max = " << worldBBox.max() << std::endl;
}

void rotateVolume(openvdb::GridBase::Ptr grid, float angleDegrees) {
    float angleRadians = angleDegrees * (3.14159265359f / 180.0f);

    openvdb::math::Mat3s rotationMatrix;
    auto rot_axis = openvdb::math::Vec3d(0, 0, 1);
    openvdb::math::Transform::Ptr transform = grid->transformPtr();
    transform->postRotate(angleRadians, openvdb::math::Z_AXIS);
    printf("Rotated by %f deg.\n", angleDegrees);
}

void translateVolume(openvdb::GridBase::Ptr grid, float x, float y, float z) {
    openvdb::math::Mat3s rotationMatrix;
    openvdb::math::Transform::Ptr transform = grid->transformPtr();
    transform->postTranslate(openvdb::math::Vec3d(x, y, z));
    printf("Translate by %f, %f, %f.\n", x, y, z);
}

int main(int argc, char** argv)
{
    try {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <input.vdb> <output.nvdb>\n";
            return 1;
        }

        std::string field_name = "density";
        if (argc >= 4) {
            field_name = argv[3];
        }

        openvdb::initialize();
        openvdb::io::File file(argv[1]);
        if (!file.open()) {
            throw std::runtime_error("Failed to open VDB file");
        }

        openvdb::GridPtrVecPtr grids = file.getGrids();
        if (!grids || grids->empty()) {
            throw std::runtime_error("No grids found in VDB file");
        }

        openvdb::FloatGrid::Ptr floatGrid = nullptr;
        int cnt_grid = 0;
        for (const auto& baseGrid : *grids) {
            // std::cout << baseGrid->getName() << std::endl;
            if (baseGrid->getName() == field_name) {
                floatGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
                std::cout << "Grid '" << field_name << "' found and converted.\n";
            }
            if (floatGrid) {
                auto cast_grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
                if (cast_grid && cast_grid->getName() == "flame") {
                    float v_min, v_max;
                    cast_grid->evalMinMax(v_min, v_max);
                    
                }
            }
        }

        float v_min, v_max;
        if (floatGrid)
            floatGrid->evalMinMax(v_min, v_max);
        else {
            std::cerr << "Float grid is nullptr. Exiting...\n";
            exit(0);
        }
        printf("Value extrema: Min: %f, Max: %f\n", v_min, v_max);
        readWorldSpaceBoundingBox(floatGrid);

        if (!floatGrid) {
            throw std::runtime_error("No FloatGrid found in VDB file");
        }

        auto nanoHandle = nanovdb::openToNanoVDB(floatGrid,
                                                 nanovdb::StatsMode::Default,
                                                 nanovdb::ChecksumMode::Default, 0);

        if (!nanoHandle) {
            throw std::runtime_error("Failed to convert to NanoVDB format");
        }

        // 写入NVDB文件
        std::ofstream outFile(argv[2], std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to create output file");
        }

        outFile.write(reinterpret_cast<const char*>(nanoHandle.data()), nanoHandle.size());

        std::cout << "Successfully converted to NanoVDB format\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}