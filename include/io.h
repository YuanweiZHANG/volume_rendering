#ifndef IO_H
#define IO_H

#include <openvdb/openvdb.h>
#include <glm/glm.hpp>

#include <vector>
#include <fstream>


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

/**
 * @brief Given data and head, save CSV file on path
 * 
 * @param data CSV data
 * @param head the head of CSV file
 * @param path saving path
 */
void saveCSV(const std::vector<float> &data, const std::vector<std::string> &head, std::string path) {
    std::ofstream outCSV;
    outCSV.open(path, std::ios::out);

    int n = data.size() / head.size();
    int headN = head.size();

    outCSV << head[0];
    for (int i = 1; i < headN; ++i) {
        outCSV << "," << head[i];
    }
    outCSV << std::endl;

    for (int i = 0; i < n; ++i) {
        outCSV << data[i * headN];
        for (int j = 1; j < headN; ++j) {
            outCSV << "," << data[i * headN + j];
        }
        outCSV << std::endl;
    }

    outCSV.close();
}

void VDB2CSV(const std::string &vdbPath, const std::string &csvPath) {
    std::vector<float> density;
    readGrid(vdbPath, "density", density);
    saveCSV(density, {"x", "y", "z", "density"}, csvPath);
}

#endif
