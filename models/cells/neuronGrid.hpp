#ifndef NEURON_CELL_HPP
#define NEURON_CELL_HPP

#include <cmath>
#include "neuronState.hpp"
#include "../../types/imageStructures.hpp"
#include "../../utils/generalUtils.hpp"
#include <cadmium/celldevs/grid/cell.hpp>
#include <cadmium/celldevs/grid/config.hpp>
#include "../../utils/stochastic/random.hpp"
#include <cstdlib> // For abs()

using namespace cadmium::celldevs;

class NeuronCell : public GridCell<NeuronState, double> {
public:
    // Constructor
    NeuronCell(const std::vector<int>& id, 
               const std::shared_ptr<const GridCellConfig<NeuronState, double>>& config, 
               int as, 
               const std::shared_ptr<WeightMatrix> wm, 
               const std::shared_ptr<StateMatrix> ns, 
               int length, 
               int width, 
                double time): GridCell<NeuronState, double>(id, config) {
                this->state.activationStatus = as;
                this->state.localWeights = wm;
                this->state.neighboringStates = ns;
                this->state.imageLength = length;
                this->state.imageWidth = width;
                this->state.time = time;
            }
            
            [[nodiscard]] NeuronState localComputation(NeuronState state,
                const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const {
                if (state.time != 0) {
                    // Get current coords
                    auto [x, y] = GeneralUtils::stringToIndices(this->getId());
                    // Update state
                    updateCellState(x,y,neighborhood,state);
                }
                // ΔE=∣Ecurrent−E prev​∣
                // Advance time
                state.time += RandomNumberGeneratorDEVS::generateExponentialDelay(1);
                // Return current state which is passed to neighbors ports
                return state;
            }

            // [[nodiscard]] NeuronState localComputation(NeuronState state,
            //     const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const {
            //         // Get current cell coords
            //         auto [x, y] = GeneralUtils::stringToIndices(this->getId());
                    
            //         // Update cell state (modifies activationStatus)
            //         updateCellState(x, y, neighborhood, state);

            //         // Compute energy difference for convergence check
            //         double E_prev = state.prevEnergy;
            //         double E_current = 0.0;
                    
            //         for (const auto& [neighborId, neighborData] : neighborhood) {
            //             int neighborFlatIndex = neighborId[0] * state.imageWidth + neighborId[1];
            //             int selfFlatIndex = x * state.imageWidth + y;
            //             E_current += -0.5 * state.localWeights->getWeightAt(selfFlatIndex, neighborFlatIndex) *
            //                         state.activationStatus * neighborData.state->activationStatus;
            //         }

            //         // Store energy for next iteration
            //         state.prevEnergy = E_current;

            //         // Compute energy difference
            //         double energyDiff = fabs(E_current - E_prev);
                    
            //         // Adaptive time update based on stability
            //         state.time += (energyDiff < 0.001) ? 0.1 : 1.0;

            //     return state;
            // }

            
            bool verifyImmidiateNeighborsHaveFired(int x, int y, const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood, NeuronState& state) const {
                int WIDTH_EDGE = state.imageWidth - 1;
                int LENGTH_EDGE = state.imageLength - 1;
                // Right Nearest 
                auto rightNearest = (x < WIDTH_EDGE)  ?  GeneralUtils::parseCellIndexToCadmiumId(x + 1, y) : "";
                // Left Nearest 
                auto leftNearest = (x > 0) ? GeneralUtils::parseCellIndexToCadmiumId(x - 1, y) : "";
                // Top Nearest 
                auto topNearest = (y > 0) ? GeneralUtils::parseCellIndexToCadmiumId(x , y - 1) : "";
                // Bottom Nearest 
                auto bottomNearest = (y < LENGTH_EDGE) ? GeneralUtils::parseCellIndexToCadmiumId(x , y + 1) : "";
            
                // Check if any of our immidiate neighbors are active.
                bool isActive = false;
                for (const auto& [neighborId, neighborData] : neighborhood) {
                    int neighborState = neighborData.state->activationStatus;
                    auto neighborStringID = GeneralUtils::parseCellIndexToCadmiumId(neighborId[0], neighborId[1]);
                    if (neighborState == 1 && 
                        (neighborStringID == rightNearest || neighborStringID == leftNearest || 
                        neighborStringID == topNearest || neighborStringID == bottomNearest)) {
                        isActive = true;
                        break;
                    }
                }
            
                return isActive;
            };

            bool neighborhoodIsActive(const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const {
                bool isActive = false;
                for (const auto& [neighborId, neighborData] : neighborhood) {
                    int neighborState = neighborData.state->activationStatus;
                    if (neighborState == 1) {
                        isActive = true;
                        break;
                    }
                }
                return isActive;
            }
            
            
            // void updateCellState(int x, int y ,const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood, NeuronState& state) const {
            //     // NxN matrix 
            //     // i.e row 0 represents the product of cell 0 with every other cell including itself. 
            //     // Each column represents the other cell i.e col 1 = cell0 * cell1, col 2 = cell0 * cell2 ... etc
            //     double sum = 0;
            //     int selfFlatIndex = x * state.imageWidth + y;
            //     // Iterate through neighboring neurons
            //     for (const auto& [neighborId, neighborData] : neighborhood) {
            //         int neighborState = neighborData.state->activationStatus;
            
            //         auto neighborStringID = GeneralUtils::parseCellIndexToCadmiumId(neighborId[0], neighborId[1]);
            //         int neighborFlatIndex = neighborId[0] * state.imageWidth + neighborId[1];
            
            //         // Perform update rule
            //         if (neighborStringID != this->getId()) {
            //             sum += state.localWeights->getWeightAt(selfFlatIndex, neighborFlatIndex) * static_cast<double>(neighborState);
            //         }
            //     }
            //     // Update activation state using the sum
            //     state.activationStatus = GeneralUtils::signum(sum);
            //     // Set it in the global state matrix
            //     state.neighboringStates->stateMatrix(x,y) = state.activationStatus;
            // }

            // Adaptive Weights - Oja's
            void updateCellState(int x, int y, 
                const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood, 
                NeuronState& state) const {
                double sum = 0.0;
                int selfFlatIndex = x * state.imageWidth + y;
                double lambda = 0.1;  // Learning rate for Hebbian update
                double decayFactor = 0.99; // Decay weight over time

                // Iterate through neighboring neurons
                for (const auto& [neighborId, neighborData] : neighborhood) {
                int neighborState = neighborData.state->activationStatus;
                int neighborFlatIndex = neighborId[0] * state.imageWidth + neighborId[1];

                if (neighborFlatIndex != selfFlatIndex) {
                    // Fetch existing weight
                    double currentWeight = state.localWeights->getWeightAt(selfFlatIndex, neighborFlatIndex);

                    // Hebbian update with decay
                    double updatedWeight = decayFactor * currentWeight + lambda * state.activationStatus * neighborState;

                    // Normalize weight to prevent divergence
                    updatedWeight /= std::max(1.0, fabs(updatedWeight));

                    // Store updated weight
                    state.localWeights->weightMatrix(selfFlatIndex, neighborFlatIndex) =  updatedWeight;

                    // Sum for activation update
                    sum += updatedWeight * neighborState;
                }
                }

                // Update activation state using sign function
                state.activationStatus = GeneralUtils::signum(sum);

                // Store new state in the global state matrix
                state.neighboringStates->stateMatrix(x, y) = state.activationStatus;
                }
            
            [[nodiscard]] double outputDelay(const NeuronState& state) const {
                return state.time;
            }
            
            NeuronState& getState(){ return this->state; }
        
};

#endif



		// bool isActive = false;
		// // Check if neighbors are valid.. 
		// for (const auto& [neighborId, neighborData] : neighborhood) {
		// 	int neighborState = neighborData.state->activationStatus;
		// 	if ( neighborState == 1 ) {
		// 		isActive = true;
		// 		break;
		// 	}
		// }

		// if (isActive == false) {
		// 	return state;
		//  }


		// for (int i =0; i < 28; i++){
		// 	for (int j =0; j < 28; j++){
		// 		sum += state.localWeights->weightMatrix[selfFlatIndex][i*28 + j] * (*state.neighboringStates)[i][j];
		// 	}
		// }

		// (*state.neighboringStates)[index1][index2] =  state.activationStatus;