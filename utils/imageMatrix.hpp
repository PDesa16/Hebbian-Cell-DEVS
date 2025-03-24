#ifndef IMAGE_MATRIX_HPP
#define IMAGE_MATRIX_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <Eigen/Dense>

enum class Sign { NEGATIVE = -1, POSITIVE = 1 };

constexpr int operator*(Sign lhs, Sign rhs) {
    return static_cast<int>(lhs) * static_cast<int>(rhs);
}

constexpr int operator+(Sign lhs, Sign rhs) {
    return static_cast<int>(lhs) + static_cast<int>(rhs);
}

constexpr int operator-(Sign lhs, Sign rhs) {
    return static_cast<int>(lhs) - static_cast<int>(rhs);
}

class ImageMatrix {
public:
    int row;
    int col;
    Eigen::MatrixXd pixels;
    Eigen::VectorXd imageFlatten;

    explicit ImageMatrix(const std::vector<std::vector<unsigned char>>& imageRaw)
        : row(imageRaw.size()), col(imageRaw[0].size()), pixels(row, col), imageFlatten(row*col) {
        ParseImage(imageRaw);
    }

    void ParseImage(const std::vector<std::vector<unsigned char>>& imageRaw) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                int pixelStr = static_cast<int>(imageRaw[i][j]);
                double value = (pixelStr > 127) ? 1.0 : -1.0;
                pixels(i, j) = value;
                imageFlatten((i* row) + j) = value;
            }
        }
    }

    double getWeightAt(int r, int c) const {
        return pixels(r, c);
    }
};

class WeightVector {
public:
    // String is the position on the pixel grid i.e [0,0] ; This format is the one used in cadmium
    std::unordered_map<std::string, double> vectorMap;

    WeightVector() = default;

    WeightVector(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices,  std::vector<int> coordinates, int range = 1) {
        calculateNeighboringWeightsAtCellIndex(imageMatrices, coordinates, range);
    }

    void calculateNeighboringWeightsAtCellIndex(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices, std::vector<int> coordinates, int range = 1) {

        // Get size of imageMatrices, tells us how many images are present
        double size = imageMatrices.size();
        // Calculate scaling factor
        double factor = 1 / size; 
        // Loop through images
        for (const auto& image : imageMatrices) {
            auto flattenIndex = (coordinates[0] + coordinates[1]*28);
            for (auto [direction, j] : vonNeumannNeighbors(flattenIndex, 28, 28, range)) {
                    // Left, Right, Up, Down 
                    // Add SCALING..
                    vectorMap[direction] += factor * (image->imageFlatten(flattenIndex) * image->imageFlatten(j));
            }
        }
    }

    std::string getNeighboringCellIndexMatrixView(int xIndex, int yIndex) {
        return "[" + std::to_string(xIndex) + "," + std::to_string(yIndex) + "]";
    }

    // std::vector<std::pair<std::string, int>> vonNeumannNeighbors(int index, int width, int height) {
    //     int x = index % width;
    //     int y = index / width;
    //     std::vector<std::pair<std::string, int>> neighbors;
    
    //     if (x > 0) {
    //         int neighborIdx = index - 1;
    //         int nx = neighborIdx % width;
    //         int ny = neighborIdx / width;
    //         neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx }); // left
    //     }
    
    //     if (x < width - 1) {
    //         int neighborIdx = index + 1;
    //         int nx = neighborIdx % width;
    //         int ny = neighborIdx / width;
    //         neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx }); // right
    //     }
    
    //     if (y > 0) {
    //         int neighborIdx = index - width;
    //         int nx = neighborIdx % width;
    //         int ny = neighborIdx / width;
    //         neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx }); // up
    //     }
    
    //     if (y < height - 1) {
    //         int neighborIdx = index + width;
    //         int nx = neighborIdx % width;
    //         int ny = neighborIdx / width;
    //         neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx }); // down
    //     }
    
    //     return neighbors;
    // }
    std::vector<std::pair<std::string, int>> vonNeumannNeighbors(int index, int width, int height, int range = 1) {
        int x = index % width;
        int y = index / width;
        std::vector<std::pair<std::string, int>> neighbors;
    
        for (int dx = -range; dx <= range; ++dx) {
            for (int dy = -range; dy <= range; ++dy) {
                if (std::abs(dx) + std::abs(dy) <= range && (dx != 0 || dy != 0)) {
                    int nx = x + dx;
                    int ny = y + dy;
    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int neighborIdx = ny * width + nx;
                        neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx });
                    }
                }
            }
        }
    
        return neighbors;
    }


    std::vector<std::pair<std::string, int>> mooreNeighbors(int index, int width, int height, int range = 1) {
        int x = index % width;
        int y = index / width;
        std::vector<std::pair<std::string, int>> neighbors;
    
        for (int dx = -range; dx <= range; ++dx) {
            for (int dy = -range; dy <= range; ++dy) {
                if (dx != 0 || dy != 0) {
                    int nx = x + dx;
                    int ny = y + dy;
    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int neighborIdx = ny * width + nx;
                        neighbors.push_back({ getNeighboringCellIndexMatrixView(nx, ny), neighborIdx });
                    }
                }
            }
        }
    
        return neighbors;
    }

    bool operator!=(const WeightVector& other) const {
        return vectorMap != other.vectorMap;
    }
    
};

#endif