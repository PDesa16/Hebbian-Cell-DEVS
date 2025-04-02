#include <gtest/gtest.h>
#include "../neuronGrid.hpp"


using namespace cadmium::celldevs;

class NeuronGridAtomicFixture: public ::testing::Test
{
protected:
    std::shared_ptr<NeuronCell> neuronCell;
    void SetUp() override 
    {
        InitModel();
    }

    void InitModel() 
    {
        const std::vector<int>& id = {0,0};
        const std::shared_ptr<const GridCellConfig<NeuronState, double>>& config = nullptr;
        const int as = -1;
        const std::shared_ptr<WeightMatrix> wm = nullptr;
        const std::shared_ptr<StateMatrix> ns = nullptr;
        const int length = 28;
        const int width = 28;
        neuronCell = std::make_shared<NeuronCell>(id, config, as, wm, ns, length, width);
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
