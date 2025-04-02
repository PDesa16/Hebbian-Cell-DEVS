#ifndef NEURON_STATE_HPP
#define NEURON_STATE_HPP

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include "../../types/imageStructures.hpp"

struct NeuronState {
public:
	int activationStatus;
	int imageWidth;
	int imageLength;
	double time;
	double prevEnergy = 0;
	std::shared_ptr<WeightMatrix> localWeights;
	std::shared_ptr<StateMatrix> neighboringStates;
	NeuronState() = default;
};

std::ostream& operator<<(std::ostream& os, const NeuronState& s) {
	os << std::to_string(s.activationStatus);
	return os;
};

inline bool operator!=(const NeuronState& x, const NeuronState& y) {
	return (x.activationStatus != y.activationStatus) || (x.time != y.time);
};

[[maybe_unused]] void from_json(const nlohmann::json& j, NeuronState& s) {

};


#endif