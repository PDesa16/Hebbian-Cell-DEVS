#include <gtest/gtest.h>
#include "../modelBuilder.hpp"

std::vector<std::shared_ptr<ImageMatrix>> images;
std::shared_ptr<WeightMatrix> globalWeightMatrix;
std::shared_ptr<StateMatrix> globalStateMatrix;

std::shared_ptr<GridCell<NeuronState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    // Create a mutable copy of the original config 
    auto modifiableConfig = std::make_shared<GridCellConfig<NeuronState, double>>(*cellConfig);
    double slightPermutation = images[0]->pixels(cellId[0], cellId[1]);

    if (cellId[0] % 2 == 0){
        slightPermutation *= -1 ;
    }

    if (modifiableConfig) {
        modifiableConfig->state.activationStatus =  images[0]->pixels(cellId[0], cellId[1]);
        globalStateMatrix->stateMatrix(cellId[0],cellId[1]) =  images[0]->pixels(cellId[0], cellId[1]);
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

class ModelBuilderAtomicFixture: public ::testing::Test
{
protected:
    std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel;

    void SetUp() override 
    {
        InitModel();
    }

    void InitModel() 
    {
        ModelBuilder modelBuilder("simulation_config.json");
        modelBuilder.loadImages().initializeWeightMatrix().initializeStateMatrix();
        globalStateMatrix = modelBuilder.globalStateMatrix;
        globalWeightMatrix = modelBuilder.globalWeightMatrix;
        images = modelBuilder.images;
        // Pass images to the weight inits..
        modelBuilder.buildNeuronCellModel(addGridCell);  
        modelBuilder.buildRootCoordinator();     
    }

};

TEST_F(ModelBuilderAtomicFixture, TestNeuronCellModelInit) {
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}
