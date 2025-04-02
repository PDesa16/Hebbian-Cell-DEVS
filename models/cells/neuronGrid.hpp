#ifndef NEURON_CELL_HPP
#define NEURON_CELL_HPP

#include <cmath>
#include "neuronState.hpp"
#include "../../types/imageStructures.hpp"
#include "../../utils/generalUtils.hpp"
#include <cadmium/celldevs/grid/cell.hpp>
#include <cadmium/celldevs/grid/config.hpp>
#include "../../utils/stochastic/random.hpp"
#include <cstdlib> 

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

    // Default added for testing
    NeuronCell() = default;
            
            [[nodiscard]] NeuronState localComputation(NeuronState state,
                const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood) const {
                if (state.time != 0) {
                    // Get current coords
                    auto [x, y] = GeneralUtils::stringToIndices(this->getId());
                    // Update state
                    updateCellState(x,y,neighborhood,state);
                }
                // Advance time
                state.time += RandomNumberGeneratorDEVS::generateExponentialDelay(1);
                // Return current state which is passed to neighbors ports
                return state;
            }
            
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
            
            
            void updateCellState(int x, int y ,const std::unordered_map<std::vector<int>, NeighborData<NeuronState, double>>& neighborhood, NeuronState& state) const {
                // NxN matrix 
                // i.e row 0 represents the product of cell 0 with every other cell including itself. 
                // Each column represents the other cell i.e col 1 = cell0 * cell1, col 2 = cell0 * cell2 ... etc
                double sum = 0;
                int selfFlatIndex = x * state.imageWidth + y;
                // Iterate through neighboring neurons
                for (const auto& [neighborId, neighborData] : neighborhood) {
                    int neighborState = neighborData.state->activationStatus;
            
                    auto neighborStringID = GeneralUtils::parseCellIndexToCadmiumId(neighborId[0], neighborId[1]);
                    int neighborFlatIndex = neighborId[0] * state.imageWidth + neighborId[1];
            
                    // Perform update rule
                    if (neighborStringID != this->getId()) {
                        sum += state.localWeights->getWeightAt(selfFlatIndex, neighborFlatIndex) * static_cast<double>(neighborState);
                    }
                }
                // Update activation state using the sum
                state.activationStatus = GeneralUtils::signum(sum);
                // Set it in the global state matrix
                state.neighboringStates->stateMatrix(x,y) = state.activationStatus;
            }
            
            [[nodiscard]] double outputDelay(const NeuronState& state) const {
                return state.time;
            }
            
            NeuronState& getState(){ return this->state; }
        
};

#endif
