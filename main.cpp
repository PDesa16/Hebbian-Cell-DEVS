#include <iostream>
#include "models/builder/modelBuilder.hpp"
#include "utils/stochastic/random.hpp"
#include "utils/generalUtils.hpp"

// Requires global access for addGridCell Factory method
std::vector<std::shared_ptr<ImageMatrix>> images;
std::shared_ptr<WeightMatrix> globalWeightMatrix;
std::shared_ptr<StateMatrix> globalStateMatrix;
int imageSelection;

std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    // Create a mutable copy of the original config 
    auto modifiableConfig = std::make_shared<GridCellConfig<NeuronState, double>>(*cellConfig);
    double slightPermutation = images[imageSelection]->pixels(cellId[0], cellId[1]);
    if (cellId[0] % 2 == 0){
        slightPermutation *= -1 ;
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  slightPermutation;
    } else {
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  images[imageSelection]->pixels(cellId[0], cellId[1]);
    }

    if (modifiableConfig) {
        modifiableConfig->state.activationStatus =  images[imageSelection]->pixels(cellId[0], cellId[1]);
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

// Simple function to print eigen matrix of doubles
void printMatrix(const Eigen::MatrixXd& matrix) {
    std::cout << matrix << std::endl;
}
// Simple function to print eigen matrix of intergers
void printMatrixi(const Eigen::MatrixXi& matrix) {
    std::cout << matrix << std::endl;
}

int main() {
    ModelBuilder modelBuilder("simulation_config.json");

    modelBuilder.loadImages() \
                .initializeWeightMatrix() \
                .initializeStateMatrix();
     
    // Set globals to builder instance, REQUIRED for factory method
    globalStateMatrix = modelBuilder.globalStateMatrix;
    globalWeightMatrix = modelBuilder.globalWeightMatrix;
    images = modelBuilder.images;
    imageSelection = modelBuilder.scenario.simulationDetails.imageSelectionIndex;

    // Verify proper indexing
    if (imageSelection > images.size()) {
        std::cerr << "Error: Invalid Image Index in Simulation Config" << std::endl;
        return 0;
    }

    // View Selection
    std::cout << "Raw Image" << std::endl;
    printMatrix(images[imageSelection] -> pixels);

    // Add factory method to builder
    modelBuilder.buildNeuronCellModel(addGridCell)\
    .buildLogger("hebbian_logfile.csv");

    // View Matrix after mixing some bits
    std::cout << "Image after altering some bits" << std::endl;
    printMatrixi(globalStateMatrix->stateMatrix);

    // Build the root coordinator
    modelBuilder.buildRootCoordinator();
    // Start Simulation
    modelBuilder.startSimulation();

    // View Simulation Result
    std::cout << "Final Image recall" << std::endl;
    printMatrixi(globalStateMatrix->stateMatrix);

    // Print experiment details
    auto finalEnergy = GeneralUtils::calculateEnergyCostFunction(globalWeightMatrix, globalStateMatrix);
    std::cout << "Energy: " << finalEnergy << " Type: " << modelBuilder.scenario.defaultCell.neighborhood[0].type << " Size: "  \
    << modelBuilder.scenario.defaultCell.neighborhood[0].range << std::endl;    
}