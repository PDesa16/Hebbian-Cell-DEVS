# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -O0 --std=c++17 -fno-common

# Directories
VENDOR_DIR = vendor
INCLUDE_DIRS = -I$(VENDOR_DIR)/cadmium_v2/include \
               -I$(VENDOR_DIR)/googletest/googletest/include \
               -I$(VENDOR_DIR)/googletest/googlemock/include \
			   -I$(VENDOR_DIR)/json/include \
			   -I$(VENDOR_DIR)/eigen

LIB_DIRS = -L$(VENDOR_DIR)/googletest/lib 

GTEST_LIBS = $(VENDOR_DIR)/googletest/lib/libgtest.a \
             $(VENDOR_DIR)/googletest/lib/libgtest_main.a

BIN_DIR = bin
SRC_DIR = models
UTILS = utils

MAIN_DEPENDENCIES = types/imageStructures.cpp utils/stochastic/random.cpp


# Test targets
TESTS = test_neuron_cell

# Build and run all tests
all: $(TESTS) run_tests

# Modify the build command to explicitly link gtest and gtest_main
build_test_neuron_cell:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) utils/stochastic/random.cpp \
	types/imageStructures.cpp \
	$(SRC_DIR)/cells/test/neuron_test.cpp $(LIB_DIRS) -lgtest -lgtest_main -o $(BIN_DIR)/test_neuron_cell

build_test_neuron_state:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(SRC_DIR)/cells/neuronState.cpp  \
	types/imageStructures.cpp \
	$(SRC_DIR)/cells/test/neuronState_test.cpp $(LIB_DIRS) -lgtest -lgtest_main -o $(BIN_DIR)/test_neuron_state

build_test_neuron_cell_grid:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) types/imageStructures.cpp \
	$(SRC_DIR)/cells/test/neuronGrid_test.cpp $(LIB_DIRS) -lgtest -lgtest_main -o $(BIN_DIR)/test_neuron_cell_grid

build_test_utils:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(UTILS)/test/utils_test.cpp $(LIB_DIRS) -lgtest -lgtest_main -o $(BIN_DIR)/test_utils

build_model_builder_test:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(MAIN_DEPENDENCIES) types/imageStructures.cpp $(SRC_DIR)/builder/test/modelBuilder_test.cpp $(LIB_DIRS) -lgtest -lgtest_main -o $(BIN_DIR)/test_model_builder

build_simulation:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(MAIN_DEPENDENCIES) main.cpp $(LIB_DIRS) -o $(BIN_DIR)/simulation


# Run all tests
run_tests: $(addprefix run_, $(TESTS))

run_test_neuron_cell:
	$(BIN_DIR)/test_neuron_cell

run_test_utils:
	$(BIN_DIR)/test_utils

.PHONY: all $(TESTS) run_tests

clean:
	rm -rf $(BIN_DIR)/*

build_all: build_test_neuron_cell
