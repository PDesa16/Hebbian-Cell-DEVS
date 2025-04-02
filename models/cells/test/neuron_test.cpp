#include <gtest/gtest.h>
#include <cadmium/core/logger/csv.hpp>
#include "../neuronGrid.hpp"
#include "../neuronState.hpp"
#include <cadmium/celldevs/grid/coupled.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../../../utils/imageLoader.hpp"
#include "../../../utils/generalUtils.hpp"
#include "../../types/imageStructures.hpp"
// #include "../../logger/EnergyLogger.hpp"


std::vector<std::shared_ptr<ImageMatrix>> images;
std::shared_ptr<WeightMatrix> globalWeightMatrix;
std::shared_ptr<StateMatrix> globalStateMatrix = std::make_shared<StateMatrix>(28, 28);

void printMatrix(const Eigen::MatrixXd& matrix) {
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

std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    // Create a mutable copy of the original config 
    auto modifiableConfig = std::make_shared<GridCellConfig<NeuronState, double>>(*cellConfig);
    double slightPermutation = images[1]->pixels(cellId[0], cellId[1]);

    if (cellId[0] % 2 == 0){
        slightPermutation *= -1 ;
    }

    if (modifiableConfig) {
        modifiableConfig->state.activationStatus =  images[1]->pixels(cellId[0], cellId[1]);
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  images[1]->pixels(cellId[0], cellId[1]);
    }

    // Example condition: if the cell model is Hebbian-Learning
    if (cellConfig->cellModel == "Hebbian-Learning") {
        // Use the modified config to create a new NeuronCell
        return std::make_shared<NeuronCell>(cellId, modifiableConfig, 
                                            slightPermutation, 
                                            globalWeightMatrix,
                                            globalStateMatrix, 
                                            modifiableConfig->scenario->shape[0],
                                            modifiableConfig->scenario->shape[1]);
    } else {
        throw std::bad_typeid();
    }
}



class NeuronCellAtomicFixture: public ::testing::Test
{
protected:
    std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel;

    void SetUp() override 
    {
        InitModel();
    }

    void InitModel() 
    {
        std::vector<std::vector<std::vector<unsigned char>>> imagesRaw = ImageLoader::LoadImages(28,28);
        for (auto image : imagesRaw) {
            images.push_back(std::make_shared<ImageMatrix>(image));
        }

        globalWeightMatrix = std::make_shared<WeightMatrix>(images);

        // Pass images to the weight inits..
        neuronCellModel = std::make_shared<GridCellDEVSCoupled<NeuronState, double>>(
            "Hebbian-Learning",
            addGridCell,
            "simulation_config.json"
        );        
        neuronCellModel->buildModel();
    }

};


/* Protocol Init Tests */
TEST_F(NeuronCellAtomicFixture, TestNeuronCellModelInit) {
    std::cout << "IMAGE MATRIX:"<< std::endl;
    printMatrix(images[0] -> pixels);

    auto rootCoordinator = cadmium::RootCoordinator(neuronCellModel);
    auto logger = std::make_shared<cadmium::CSVLogger>("logs/learning.csv", ",");
    rootCoordinator.setLogger(logger);
	rootCoordinator.start();

    printNeuronStatesMatrixStyle(neuronCellModel, 28, 28);

	rootCoordinator.simulate(5.0);
	rootCoordinator.stop();

    printNeuronStatesMatrixStyle(neuronCellModel, 28, 28);

    // auto energy = GeneralUtils::calculateEnergyCostFunction(globalWeightMatrix, globalStateMatrix);
    // EnergyLogger energyLogger("logs/energy_log.csv");
    // energyLogger.log("simulation_group_1", "image1", "von", 4, energy);

    ASSERT_TRUE(neuronCellModel != nullptr);
}

int main(int argc, char **argv) {
    std::cout << "Starting Google Test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv); 
    std::cout << "Running all tests" << std::endl;
    return RUN_ALL_TESTS();
}
