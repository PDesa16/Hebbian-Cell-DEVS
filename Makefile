# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g --std=c++17 -pthread

# Directories
VENDOR_DIR = vendor
INCLUDE_DIRS = -I$(VENDOR_DIR)/cadmium_v2/include \
               -I$(VENDOR_DIR)/googletest/googletest/include \
               -I$(VENDOR_DIR)/googletest/googlemock/include \

LIB_DIRS = -L$(VENDOR_DIR)/googletest/lib 

GTEST_LIBS = $(VENDOR_DIR)/googletest/lib/libgtest.a \
             $(VENDOR_DIR)/googletest/lib/libgtest_main.a


BIN_DIR = bin
SRC_DIR = models

# Test targets
TESTS = test_buffer

# Build and run all tests
all: $(TESTS) run_tests

build_test_raft_controller:
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(SRC_DIR)/atomic/test/raft_test.cpp \
	$(GTEST_LIBS) $(CRYPTOPP_LIBS) -o $(BIN_DIR)/test_raft_controller $(LIB_DIRS)

# Run all tests
run_tests: $(addprefix run_, $(TESTS))

run_test_buffer:
	$(BIN_DIR)/test_buffer

.PHONY: all $(TESTS) run_tests

clean:
	rm -rf $(BIN_DIR)/*

build_all: build_test_raft_controller build_test_network build_packet_processor_raft \
           build_message_processor_raft build_node build_simulation build_heartbeat_controller \
           build_raft build_buffer
