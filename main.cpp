#include <iostream>
#include "models/builder/modelBuilder.hpp"
#include "utils/stochastic/random.hpp"
#include "utils/generalUtils.hpp"


std::vector<std::shared_ptr<ImageMatrix>> images;
std::shared_ptr<WeightMatrix> globalWeightMatrix;
std::shared_ptr<StateMatrix> globalStateMatrix;

std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    // Create a mutable copy of the original config 
    auto modifiableConfig = std::make_shared<GridCellConfig<NeuronState, double>>(*cellConfig);
    double slightPermutation = images[3]->pixels(cellId[0], cellId[1]);

    if (cellId[0] % 2 == 0){
        slightPermutation *= -1 ;
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  slightPermutation;
    } else {
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  images[3]->pixels(cellId[0], cellId[1]);
    }

    if (modifiableConfig) {
        modifiableConfig->state.activationStatus =  images[3]->pixels(cellId[0], cellId[1]);
    }

    // Example condition: if the cell model is Hebbian-Learning
    if (cellConfig->cellModel == "Hebbian-Learning") {
        // Use the modified config to create a new NeuronCell
        return std::make_shared<NeuronCell>(cellId, modifiableConfig, 
                                            slightPermutation, 
                                            globalWeightMatrix,
                                            globalStateMatrix, 
                                            modifiableConfig->scenario->shape[0],
                                            modifiableConfig->scenario->shape[1],
                                            0);
    } else {
        throw std::bad_typeid();
    }
}


void printMatrix(const Eigen::MatrixXd& matrix) {
    std::cout << matrix << std::endl;
}
void printMatrixi(const Eigen::MatrixXi& matrix) {
    std::cout << matrix << std::endl;
}

void printNeuronStatesMatrixStyle(std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel, int rows, int cols) {
    Eigen::MatrixXi stateMatrix(rows, cols); // Integer matrix for -1 and +1

    auto set = neuronCellModel->getComponents();

    for (const auto& comp : set) {
        auto state = std::dynamic_pointer_cast<NeuronCell>(comp.second);
        int value = state->getState().activationStatus ;
        // issue here.
        auto [index1, index2] = GeneralUtils::stringToIndices(state->getId());
        stateMatrix(index1, index2) = value;
    }

    std::cout << "Slight Permutation (Matrix View):\n";
    std::cout << stateMatrix << std::endl;
}


int main() {
    ModelBuilder modelBuilder("simulation_config.json");


    // Loop through the simulation neighborhoods 
    // Store the simulation results

    modelBuilder.loadScenario() \
                .loadImages() \
                .initializeWeightMatrix() \
                .initializeStateMatrix();
     
    globalStateMatrix = modelBuilder.globalStateMatrix;
    globalWeightMatrix = modelBuilder.globalWeightMatrix;
    images = modelBuilder.images;

    // printMatrix(images[0] -> pixels);

    modelBuilder.buildNeuronCellModel(addGridCell)\
    .buildLogger("hebbian_logfile");

    // printMatrixi(globalStateMatrix->stateMatrix);

    // printNeuronStatesMatrixStyle(modelBuilder.neuronCellModel, 28, 28);

    modelBuilder.buildRootCoordinator();
    // Start Simulation
    modelBuilder.startSimulation();

    printMatrixi(globalStateMatrix->stateMatrix);

    auto finalEnergy = GeneralUtils::calculateEnergyCostFunction(globalWeightMatrix, globalStateMatrix);
    std::cout << "Energy: " << finalEnergy << " Type: " << modelBuilder.scenario.defaultCell.neighborhood[0].type << " Size: "  \
    << modelBuilder.scenario.defaultCell.neighborhood[0].range << std::endl;

    std::cout << "test" << std::endl;       
}