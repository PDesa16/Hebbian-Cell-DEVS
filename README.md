# Project Setup and Usage

## Cloning the Repository
To clone this repository along with its submodules, run:
```sh
git clone --recurse-submodules https://github.com/PDesa16/Hebbian-Cell-DEVS
```

## Traverse to Project Directory
```sh
cd Hebbian-Cell-DEVS
```
## Give Permissions to the Build Script
```sh
chmod 755 build_script.sh
```


## Building Dependencies
Before building the project, install the necessary dependencies by running:
```sh
./build_dependencies.sh
```

## Building the Project
To build all tests, use:
```sh
make build_all
```

You can also build individual components using:
```sh
make build_test_neuron_state
make build_test_neuron_cell_grid
make build_test_utils
make build_test_model_builder
make build_simulation
```

## Running Tests
To run all tests:
```sh
make run_tests
```

To run specific tests:
```sh
make run_test_neuron_state
make run_test_neuron_cell_grid
make run_test_utils
make run_test_model_builder
```

## Running the Simulation
To run the simulation test, execute:
```sh
make run_simulation
```

## Cleaning Up
To clean up compiled binaries, run:
```sh
make clean
```
