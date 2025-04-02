#ifndef MODEL_BUILDER_HPP
#define MODEL_BUILDER_HPP

#include <iostream>
#include "../../config/simulationConfig.hpp"
#include <cadmium/core/logger/csv.hpp>
#include "../cells/neuronGrid.hpp"
#include "../cells/neuronState.hpp"
#include <cadmium/celldevs/grid/coupled.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>
#include "../../utils/imageLoader.hpp"
#include "../../utils/generalUtils.hpp"
#include "../../types/imageStructures.hpp"
#include "../../logger/EnergyLogger.hpp"

class ModelBuilder {
public:
    JSON_LOADER scenarioLoader;
    Scenario scenario;
    std::vector<std::shared_ptr<ImageMatrix>> images;
    std::shared_ptr<WeightMatrix> globalWeightMatrix;
    std::shared_ptr<StateMatrix> globalStateMatrix;
    std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> neuronCellModel;
    std::shared_ptr<cadmium::RootCoordinator> rootCoordinator;
    std::shared_ptr<cadmium::CSVLogger> logger;

    // Constructor
    ModelBuilder(const std::string& configFile) {
        // Load scenario on initialization
        scenario = scenarioLoader.loadScenario(configFile);
    }

    ~ModelBuilder() = default;

    //  Load images from raw data
    ModelBuilder& loadImages() {
        std::vector<std::vector<std::vector<unsigned char>>> imagesRaw = ImageLoader::LoadImages(scenario.shape[0], scenario.shape[1]);
        for (const auto& image : imagesRaw) {
            images.push_back(std::make_shared<ImageMatrix>(image));
        }
        return *this;
    }

    // Initialize weight matrix
    ModelBuilder& initializeWeightMatrix() {
        globalWeightMatrix = std::make_shared<WeightMatrix>(images);
        return *this;
    }

    // Initialize global state matrix
    ModelBuilder& initializeStateMatrix() {
        globalStateMatrix = std::make_shared<StateMatrix>(scenario.shape[0], scenario.shape[1]);
        return *this;
    }


    // Build the neuron cell model
    ModelBuilder& buildNeuronCellModel(
        std::shared_ptr<GridCell<NeuronState, double>> (*factoryFunction)(
            const coordinates&,
            const std::shared_ptr<const GridCellConfig<NeuronState, double>>&))
    {
        neuronCellModel = std::make_shared<GridCellDEVSCoupled<NeuronState, double>>(
            "Hebbian-Learning",
            factoryFunction,
            "simulation_config.json"
        );
        neuronCellModel->buildModel();
        return *this;
    }

    // Build the neuron cell model
    ModelBuilder& buildLogger(const std::string& logFile)
    {
        logger = std::make_shared<cadmium::CSVLogger>("logs/" + logFile, ",");
        return *this;
    }

    // Build the neuron cell model
    ModelBuilder& buildRootCoordinator()
    {
        rootCoordinator = std::make_shared<cadmium::RootCoordinator>(neuronCellModel);  
        rootCoordinator -> setLogger(logger);
        return *this;
    }

    void startSimulation()
        {
            rootCoordinator -> start();
            rootCoordinator -> simulate(5.0);
            rootCoordinator -> stop();
        }
    
    // Getter for the built neuron cell model
    std::shared_ptr<GridCellDEVSCoupled<NeuronState, double>> getNeuronCellModel() const {
        return neuronCellModel;
    }

};
#endif // MODEL_BUILDER_HPP
