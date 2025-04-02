#ifndef IMAGE_MATRIX_HPP
#define IMAGE_MATRIX_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <Eigen/Dense>

class ImageMatrix {
public:
    int row;
    int col;
    Eigen::MatrixXd pixels;
    Eigen::VectorXd imageFlatten;

    explicit ImageMatrix(const std::vector<std::vector<unsigned char>>& imageRaw)
    : row(imageRaw.size()), col(imageRaw[0].size()), pixels(row, col), imageFlatten(row * col) {
    ParseImage(imageRaw);
    }

    void ParseImage(const std::vector<std::vector<unsigned char>>& imageRaw) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                int pixelStr = static_cast<int>(imageRaw[i][j]);
                double value = (pixelStr > 127) ? 1.0 : -1.0;
                pixels(i, j) = value;
                imageFlatten((i * row) + j) = value;
            }
        }
    }

    double getWeightAt(int r, int c) const {
        return pixels(r, c);
    }

};

class WeightMatrix {
public:
    Eigen::MatrixXd weightMatrix;

    WeightMatrix() = default;

    WeightMatrix(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices) {
        calculateWeightMatrix(imageMatrices);
    }
    
    void calculateWeightMatrix(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices) {
        if (imageMatrices.empty()) return;
    
        int size = imageMatrices[0]->imageFlatten.size();
        weightMatrix = Eigen::MatrixXd::Zero(size, size);
        double factor = 1.0 / imageMatrices.size();
    
        for (const auto& imageMatrix : imageMatrices) {
            Eigen::VectorXd vec = imageMatrix->imageFlatten;
            weightMatrix += factor * (vec * vec.transpose());
        }
        // Zero out the diagonal, these are self connections, weights should not influence themselves.
        weightMatrix.diagonal().setZero();
    }
    
    double getWeightAt(int r, int c) const {
        return weightMatrix(r, c);
    }
};

class StateMatrix {
public:
    Eigen::MatrixXi stateMatrix;
    
    StateMatrix() = default;

    StateMatrix(int width, int length) {
        stateMatrix = Eigen::MatrixXi::Zero(width, length);
    }

    double getStateAt(int r, int c) const {
        return stateMatrix(r, c);
    }
};

#endif
