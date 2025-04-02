#include <gtest/gtest.h>
#include "../neuronState.hpp"

class NeuronStateAtomicFixture: public ::testing::Test
{
protected:
    NeuronState neuronState;
    void SetUp() override 
    {
        InitModel();
    }

    void InitModel() 
    {
        neuronState = NeuronState();
    }

};


TEST_F(NeuronStateAtomicFixture, TestNeuronStateModelInit) {
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    std::cout << "Starting Google Test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv); 
    std::cout << "Running all tests" << std::endl;
    return RUN_ALL_TESTS();
}
