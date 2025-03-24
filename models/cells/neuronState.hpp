#ifndef NEURON_STATE_HPP
#define NEURON_STATE_HPP

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include "../../utils/imageMatrix.hpp"


// class WeightMatrix {
// 	public:
// 		WeightMatrix(int _row, int _col) : row(_row), col(_col) { Init(); };
// 		int row;
// 		int col;
// 		std::vector<std::vector<Sign>> matrix;

// 		void Init() {
// 			for (int i =0; i<row; i++){
// 				std::vector<Sign> rowEntry;
// 				for (int j=0; j<col; j++){	
// 					// Default to negative for now
// 					rowEntry.push_back(Sign::NEGATIVE);
// 				}
// 				matrix.push_back(rowEntry);
// 			}
// 		}
// };

class NeuronState {
	public:
	int activationStatus;
    std::shared_ptr<WeightVector> localWeights;
    double time = 0;
	NeuronState(int as, std::shared_ptr<WeightVector> lw, double t) : activationStatus(as), localWeights(lw), time(t) {};
	NeuronState() = default;
};


std::ostream& operator<<(std::ostream& os, const NeuronState& s) {
	os << std::to_string(s.activationStatus);
	return os;
}

bool operator!=(const NeuronState& x, const NeuronState& y) {
	return x.activationStatus != y.activationStatus;
}

void from_json(const nlohmann::json& j, NeuronState& s) {
	// j.at("activationStatus").get_to(s.activationStatus);
}

#endif 