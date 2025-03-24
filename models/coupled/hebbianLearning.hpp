#ifndef HEBBIAN_LEARNING_HPP
#define HEBBIAN_LEARNING_HPP

#include <cadmium/core/modeling/coupled.hpp>
#include "../atomic/cost_function.hpp" 
#include "../cells/neuronCell.hpp"

using namespace cadmium;

class HebbianLearningModel : public Coupled {
public:

    explicit HebbianLearningModel(const std::string& id) : Coupled(id) {

        // Create instances of atomic models
        auto neuronGrid = addComponent<NeuronGrid>("neuron-grid");
        auto costFunctionAtomic = addComponent<CostFunctionAtomic>("cost-function");

        // Define couplings
        addCoupling(neuronGrid -> getOutPort("neurons_out"), costFunctionAtomic -> getInPort("neurons_in")); // Internal Coupling (IC)
        addCoupling(costFunctionAtomic -> getOutPort("neurons_out"), neuronGrid -> getInPort("neurons_in")); // Internal Coupling (IC)
    }
};  

#endif