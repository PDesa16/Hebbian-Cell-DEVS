#include <gtest/gtest.h>
#include <cadmium/core/logger/csv.hpp>
#include "../neuronGrid.hpp"
#include "../neuronState.hpp"
#include <cadmium/celldevs/grid/coupled.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../../../utils/imageLoader.hpp"
#include "../../../utils/imageMatrix.hpp"


std::vector<std::shared_ptr<ImageMatrix>> images;

void printMatrix(const Eigen::MatrixXd& matrix) {
    std::cout << matrix << std::endl;
}


std::tuple<int, int> stringToIndices(const std::string& neighborStringID) {
    // Remove the parentheses and split by the comma
    std::string stripped = neighborStringID.substr(1, neighborStringID.size() - 2);  // Remove "(" and ")"
    
    std::stringstream ss(stripped);
    int index1, index2;

    // Extract the two integers separated by the comma
    char comma;  // To discard the comma
    ss >> index1 >> comma >> index2;

    return std::make_tuple(index1, index2);
}

void printNeuronStatesMatrixStyle(std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel, int rows, int cols) {
    Eigen::MatrixXi stateMatrix(rows, cols); // Integer matrix for -1 and +1

    auto set = neuronCellModel->getComponents();

    for (const auto& comp : set) {
        auto state = std::dynamic_pointer_cast<NeuronCell>(comp.second);
        int value = state->getState().activationStatus ;
        // issue here.
        auto [index1, index2] = stringToIndices(state->getId());
        stateMatrix(index1, index2) = value;
    }

    std::cout << "Slight Permutation (Matrix View):\n";
    std::cout << stateMatrix << std::endl;
}

// std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {

//     auto cellModel = cellConfig->cellModel;

//     // if (cellId[0] % 28 == 0) {
//     //     // Flip State
//     //     activationStatus = images[1]->pixels(cellId[0], cellId[1]) == -1 ? Sign::POSITIVE : Sign::NEGATIVE;
//     // }

//     if (images[1]->pixels(cellId[0], cellId[1]) == 1) {
//         std:: cout << "test";
//     }


//     if (cellModel == "Hebbian-Learning") {
//         // cellConfig -> state.globalWeights = imageMatrix; 
//         return std::make_shared<NeuronCell>(cellId, cellConfig,  images[1]->pixels(cellId[0], cellId[1]), std::make_shared<WeightVector>(images, cellId, 4) , 0);
//     } else {
//         throw std::bad_typeid();
// //     }
// }


std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    // Create a mutable copy of the original config (if possible)
    auto modifiableConfig = std::make_shared<GridCellConfig<NeuronState, double>>(*cellConfig);

    // Modify the cellConfig as needed
    // For example, if you want to modify weights or other settings:
    if (modifiableConfig) {
        // Assuming you're modifying some property inside GridCellConfig
        // Example: modifiableConfig->state.globalWeights = someNewWeights; 
        // This could be whatever modification you need based on your logic
        modifiableConfig->state.activationStatus =  images[1]->pixels(cellId[0], cellId[1]);
    }

    // Example condition: if the cell model is Hebbian-Learning
    if (cellConfig->cellModel == "Hebbian-Learning") {
        // Use the modified config to create a new NeuronCell
        return std::make_shared<NeuronCell>(cellId, modifiableConfig, 
                                            images[1]->pixels(cellId[0], cellId[1]), 
                                            std::make_shared<WeightVector>(images, cellId, 4), 
                                            0);
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
        std::vector<std::vector<std::vector<unsigned char>>> imagesRaw = Utils::LoadImages();
        for (auto image : imagesRaw) {
            images.push_back(std::make_shared<ImageMatrix>(image));
        }

        // Pass images to the weight inits..
        neuronCellModel = std::make_shared<GridCellDEVSCoupled<NeuronState, double>>(
            "Hebbian-Learning",
            addGridCell,
            "config/simulationViewer_config.json"
        );        
        neuronCellModel->buildModel();
    }

};


/* Protocol Init Tests */
TEST_F(NeuronCellAtomicFixture, TestNeuronCellModelInit) {
    std::cout << "IMAGE MATRIX:"<< std::endl;
    printMatrix(images[1] -> pixels);

    auto rootCoordinator = cadmium::RootCoordinator(neuronCellModel);
    auto logger = std::make_shared<cadmium::CSVLogger>("logs/learning.csv", ",");
    rootCoordinator.setLogger(logger);
	rootCoordinator.start();

    printNeuronStatesMatrixStyle(neuronCellModel, 28, 28);

	rootCoordinator.simulate(5.0);
	rootCoordinator.stop();


    printNeuronStatesMatrixStyle(neuronCellModel, 28, 28);

    
    ASSERT_TRUE(neuronCellModel != nullptr);
}

int main(int argc, char **argv) {
    std::cout << "Starting Google Test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv); 
    std::cout << "Running all tests" << std::endl;
    return RUN_ALL_TESTS();
}
