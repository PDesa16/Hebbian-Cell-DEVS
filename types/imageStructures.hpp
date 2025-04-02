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

    explicit ImageMatrix(const std::vector<std::vector<unsigned char>>& imageRaw);

    void ParseImage(const std::vector<std::vector<unsigned char>>& imageRaw);

    double getWeightAt(int r, int c) const;
};

class WeightMatrix {
public:
    Eigen::MatrixXd weightMatrix;

    WeightMatrix();

    explicit WeightMatrix(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices);

    void calculateWeightMatrix(const std::vector<std::shared_ptr<ImageMatrix>>& imageMatrices);

    double getWeightAt(int r, int c) const;
};

class StateMatrix {
public:
    Eigen::MatrixXi stateMatrix;

    StateMatrix();

    explicit StateMatrix(int width, int length);

    double getStateAt(int r, int c) const;
};

#endif
