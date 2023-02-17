#ifndef OPENVDB_H
#define OPENVDB_H

#include <openvdb/openvdb.h>
#include <glm/glm.hpp>

#include <vector>


/**
 * @brief This function read .vdb format file and return a vector
 * 
 * @param fileName OpenVDB file's path
 * @param gridName For example, "density"
 * @param data vector<float>: [x, y, z, density]
 */
void readGrid(const std::string fileName, 
            const std::string gridName, 
            std::vector<float> & data) {
    openvdb::initialize();

    openvdb::io::File file(fileName.c_str());
    file.open();

    openvdb::GridBase::Ptr baseGrid;
    for (auto nameIter = file.beginName(); nameIter != file.endName(); ++nameIter) {
        if (nameIter.gridName() == gridName) {
            baseGrid = file.readGrid(nameIter.gridName());
        }
        else {
            std::cout << "skipping grid " << nameIter.gridName() << std::endl;
        }
    }

    file.close();

    if (!data.empty()) {
        data.clear();
    }

    openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
    for (openvdb::FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
        openvdb::Coord coord = iter.getCoord();
        data.push_back(coord.x());
        data.push_back(coord.y());
        data.push_back(coord.z());
        data.push_back(iter.getValue());
    }
}

#endif
