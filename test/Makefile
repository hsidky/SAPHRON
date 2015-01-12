# Root of gtest dir.
GTEST_DIR = ../gtest

BUILD_DIR = ../build

SRC_DIR = ../src

# Flags passed to the preprocessor.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++11 -O3

# Build dependencies
DEPS  = $(BUILD_DIR)/Ising3DModel.o

# All tests produced by this Makefile.
TESTS = RandTests.out SiteTests.out BaseModelTests.out Ising3DModelTests.out \
				FlipSpinMoveTests.out NVTEnsembleTests.out

# Google Test libs to link against.
GTEST_LIBS = $(GTEST_DIR)/lib/libgtest.a $(GTEST_DIR)/lib/libgtest_main.a

# House-keeping build targets.
all : $(DEPS) $(TESTS)

clean :
	rm -f $(TESTS) $(BUILD_DIR)/*.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/Models/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%Tests.o: %Tests.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%Tests.out: $(BUILD_DIR)/%Tests.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $(wildcard $(BUILD_DIR)/$*.o) $^ -o $@

NVTEnsembleTests.out: $(BUILD_DIR)/NVTEnsembleTests.o $(GTEST_LIBS) $(BUILD_DIR)/Ising3DModel.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
