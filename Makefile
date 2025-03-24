# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -O0 --std=c++17 

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

# Test targets
TESTS = test_neuron_cell

# Build and run all tests
all: $(TESTS) run_tests

build_test_neuron_cell:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(SRC_DIR)/cells/test/neuron_test.cpp utils/stochastic/random.cpp \
	$(GTEST_LIBS) -o $(BIN_DIR)/test_neuron_cell $(LIB_DIRS) 

build_test_utils:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(UTILS)/test/utils_test.cpp \
	$(GTEST_LIBS) -o $(BIN_DIR)/test_utils $(LIB_DIRS) 

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