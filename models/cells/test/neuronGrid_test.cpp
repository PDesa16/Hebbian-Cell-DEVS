#include <gtest/gtest.h>
#include "../neuronGrid.hpp"
#include <cadmium/celldevs/grid/coupled.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>

using namespace cadmium::celldevs;

std::shared_ptr<GridCell<NeuronState, double>> FactoryFunction(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<NeuronState, double>>& cellConfig) {
    return nullptr;
}

class NeuronGridAtomicFixture: public ::testing::Test
{
protected:
    std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel;
    std::shared_ptr<NeuronCell> neuronCell;
    void SetUp() override 
    {
        InitModel();
    }

    void InitModel() 
    {
        // Define shape and origin for the GridScenario
        coordinates shape = {10, 10};  // Grid of size 10x10
        coordinates origin = {0, 0};   // Origin at (0,0)
        bool wrapped = false;          // No wrap-around in the grid

        // Create a valid instance of GridScenario
        auto mockScenario = std::make_shared<const GridScenario>(shape, origin, wrapped);

        // Sample configuration parameters
        nlohmann::json configParams = {
            {"cell_map", std::vector<coordinates>{{0,0}, {1,1}}},  // Mock cell map
            {"neighborhood", {{{"type", "absolute"}, {"neighbors", std::vector<coordinates>{{0,1}, {1,0}}}}}}
        };

        // Create the shared_ptr instance
        auto testConfig = std::make_shared<const GridCellConfig<NeuronState, double>>("testConfig", configParams, mockScenario);

        const std::vector<int>& id = {0,0};
        const int as = -1;
        const std::shared_ptr<WeightMatrix> wm = std::make_shared<WeightMatrix>();
        const std::shared_ptr<StateMatrix> ns =  std::make_shared<StateMatrix>();
        const int length = 28;
        const int width = 28;
        const int time =0;
        neuronCell = std::make_shared<NeuronCell>(id, testConfig, as, wm, ns, length, width, time);
    }
};


TEST_F(NeuronGridAtomicFixture, TestNeuronCellModelInit) {
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    std::cout << "Starting Google Test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv); 
    std::cout << "Running all tests" << std::endl;
    return RUN_ALL_TESTS();
}
