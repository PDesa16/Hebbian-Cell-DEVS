


#ifndef GENERAL_UTILS_HPP
#define GENERAL_UTILS_HPP

#include <string>
#include <memory>
#include <sstream>
#include "../types/imageStructures.hpp"

class GeneralUtils {
public:
    static int signum(double num) {  
        return (num > 0) ? 1 : -1;
    }

    static std::string parseCellIndexToCadmiumId(int x, int y) {
        return "(" + std::to_string(x) + "," +  std::to_string(y) + ")";
    }

    static std::tuple<int, int> stringToIndices(const std::string& neighborStringID) {
       	// Remove the parentheses and split by the comma
		std::string stripped = neighborStringID.substr(1, neighborStringID.size() - 2);  // Remove "(" and ")"
		std::stringstream ss(stripped);
		int index1, index2;
		// Extract the two integers separated by the comma
		char comma;  // To discard the comma
		ss >> index1 >> comma >> index2;
	
		return std::make_tuple(index1, index2); 
    }

    static double calculateEnergyCostFunction(std::shared_ptr<WeightMatrix> globalWeightMatrix, std::shared_ptr<StateMatrix> globalStateMatrix) {
        double energy = 0;
        // Flatten state matrix
        auto flatenStateMatrix = globalStateMatrix->stateMatrix.reshaped();
        auto N = globalWeightMatrix->weightMatrix.rows();
        auto M = globalWeightMatrix->weightMatrix.cols();
        for (int i =0; i < N; i++){
			for (int j =0; j < M; j++){
				energy += globalWeightMatrix->getWeightAt(i, j) * flatenStateMatrix(i) * flatenStateMatrix(j);
			}
		}
        // Return normalized energy 
        return (-0.5 * energy) / (N*M);
    }
};

#endif
