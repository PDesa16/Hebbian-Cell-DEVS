#ifndef NEURON_CELL_HPP
#define NEURON_CELL_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/grid/cell.hpp>
#include <cadmium/celldevs/grid/config.hpp>
#include "neuronState.hpp"
#include "../../utils/imageLoader.hpp"
#include "../../utils/stochastic/random.hpp"

using namespace cadmium::celldevs;

int signum(float num) {  
    return (num > 0) ? 1 : -1;
}

//! Conway game of life cell.
class NeuronCell : public GridCell<NeuronState, double> {
	public:


	NeuronCell(const std::vector<int>& id, 
		const std::shared_ptr<const GridCellConfig<NeuronState, double>>& config, 
		const int as, const std::shared_ptr<WeightVector> wm, const double time ) 
 		: GridCell<NeuronState, double>(id, config){
			this->state.activationStatus = as;
			this->state.localWeights = wm;
		}


	// [[nodiscard]] NeuronState localComputation(NeuronState state,
	// 	const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const override {
	// 	/* Calculate the neuron state */

	// 	// Get the weight matrix for the area, say its a Von-Neumann, we have a 4x4 
	// 	// Sum the state of nearby nodes with the connection weights of our current neuron 
	// 	// If it has a positive coorelation the majority should be one

	// 	// These weights are relative to the neuron and its connecting neurons
	// 	// They are precomputed at the start of the simulation 
	// 	// Here we are updating the energy wells

	// 	int sum = 0;
	// 	double energy = -1;

	// 	std::cout << this->getId() << std::endl;

	// 	// Iterate through neighboring neurons
	// 	for (const auto& [neighborId, neighborData] : neighborhood) {
	// 		Sign neighborState = neighborData.state->activationStatus;

	// 		auto neighborStringID = "(" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + ")";
		
	// 		if (neighborStringID != this->getId()) {
	// 			sum += state.localWeights->vectorMap["[" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + "]"] * static_cast<double>(neighborState);
	// 		}
	// 	}

	// 	// Update activation state using the sign function
	// 	state.activationStatus = signum(sum);

	// 	// // Compute Energy Function
	// 	// for (const auto& [neighborId, neighborData] : neighborhood) {
	// 	// 	Sign neighborState = neighborData.state->activationStatus;
	// 	// 	energy += state.localWeights->vectorMap["[" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + "]"] * static_cast<double>(neighborState * state.activationStatus);
	// 	// }

	// 	// Update time step
	// 	state.time += RandomNumberGeneratorDEVS::generateUniformDelay(0.1,0.5);
	// 	// energy *= 0.5;

	// 	if (energy == 0) {
	// 		state.time = std::numeric_limits<double>::infinity();
	// 	}

	// 	return state;
	// }

	[[nodiscard]] NeuronState localComputation(NeuronState state,
		const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const override {
		
		// Get the weight matrix for the area (Von-Neumann neighborhood in this case)
		int sum = 0;
		double energy = -1;
	
		// std::cout << "Neuron ID: " << this->getId() << std::endl;

		// Get immidiate neighbors, will use to trigger events
		auto [index1, index2] = stringToIndices(this->getId());
		// Right Nearest 
		auto rightNearest = (index1 < 27)  ? "(" + std::to_string(index1 + 1) + "," + std::to_string(index2) + ")" : "";
		// Left Nearest 
		auto leftNearest = (index1 > 0) ? "(" + std::to_string(index1 - 1) + "," + std::to_string(index2) + ")" : "";
		// Top Nearest 
		auto topNearest = (index2 > 0) ? "(" + std::to_string(index1) + "," + std::to_string(index2 - 1) + ")" : "";
		// Bottom Nearest 
		auto bottomNearest = (index2 < 27) ? "(" + std::to_string(index1) + "," + std::to_string(index2 + 1) + ")" : "";

		// Check if any of our immidiate neighbors are active. If so update...
		// bool isActive = false;
		// for (const auto& [neighborId, neighborData] : neighborhood) {
		// 	int neighborState = neighborData.state->activationStatus;
		// 	auto neighborStringID = "(" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + ")";
		// 	std::cout << "Right Nearest" << (neighborStringID == rightNearest ? "Valid" : "Invalid") << std::endl;
		// 	std::cout << "Left Nearest" << (neighborStringID == leftNearest ? "Valid" : "Invalid") << std::endl;
		// 	std::cout << "Top Nearest" << (neighborStringID == topNearest ? "Valid" : "Invalid") << std::endl;
		// 	std::cout << "Bottom Nearest" << (neighborStringID == bottomNearest ? "Valid" : "Invalid") << std::endl;

		// 	std::cout << "Firing state: " << std::to_string(neighborState) << std::endl;


   		// 	if (neighborState == 1) {
		// 		std::cout << "Firing Near me" << std::endl;
		// 	}

			

		// 	if ( (neighborStringID == rightNearest) && (neighborState == 1)) {
		// 		isActive = true ;
		// 		break;
		// 	}

		// 	if ((neighborStringID == leftNearest) && (neighborState == 1)) {
		// 		isActive = true ;
		// 		break;
		// 	}

		// 	if ((neighborStringID == topNearest) && (neighborState == 1)) {
		// 		isActive = true ;
		// 		break;
		// 	}

		// 	if ((neighborStringID == bottomNearest) && (neighborState == 1)) {
		// 		isActive = true ;
		// 		break;
		// 	}
		// }

		// if (isActive == false) {
		// 	return state;
		// }

		// Iterate through neighboring neurons
		for (const auto& [neighborId, neighborData] : neighborhood) {
			int neighborState = neighborData.state->activationStatus;
	
			auto neighborStringID = "(" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + ")";
	
			// Check if the current neuron is receiving activation from a neighbor in order
			if (neighborStringID != this->getId()) {
				// // Only activate if the left or top neighbor has fired
				sum += state.localWeights->vectorMap["[" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + "]"] * static_cast<double>(neighborState);
			}
		}
	
		// Update activation state using the sum
		state.activationStatus = signum(sum);
	
		// Compute Energy Function (if needed)
		for (const auto& [neighborId, neighborData] : neighborhood) {
			int neighborState = neighborData.state->activationStatus;
			energy += state.localWeights->vectorMap["[" + std::to_string(neighborId[0]) + "," +  std::to_string(neighborId[1]) + "]"] * static_cast<double>(neighborState * state.activationStatus);
		}
	
		// Add randomness to the time delay between activations
		state.time += 0.0001;
	
		// // // If energy is zero, stop the process (or set to infinity)
		// if (energy == 0) {
		// 	state.time = std::numeric_limits<double>::infinity();
		// }
	
		return state;
	}
	

	[[nodiscard]] double outputDelay(const NeuronState& state) const override {
		return state.time;
	}
	
	NeuronState& getState(){
		return this->state;
	}

	std::tuple<int, int> stringToIndices(const std::string& neighborStringID) const {
		// Remove the parentheses and split by the comma
		std::string stripped = neighborStringID.substr(1, neighborStringID.size() - 2);  // Remove "(" and ")"
		
		std::stringstream ss(stripped);
		int index1, index2;
	
		// Extract the two integers separated by the comma
		char comma;  // To discard the comma
		ss >> index1 >> comma >> index2;
	
		return std::make_tuple(index1, index2);
	}

};


#endif 