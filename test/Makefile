# Root of gtest dir.
GTEST_DIR = ../gtest

BUILD_DIR = ../build

LIB_DIR  = ../lib

SRC_DIR = ../src

CXX = clang++

# Flags passed to the preprocessor.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++11 -O3

# Build dependencies
DEPS  = $(LIB_DIR)/SAPHRON.a

# All tests produced by this Makefile.
TESTS = RandTests.out SiteTests.out BaseModelTests.out Ising3DModelTests.out \
	    FlipSpinMoveTests.out NVTEnsembleTests.out CSVLoggerTests.out \
		LebwohlLasherModelTests.out SphereUnitVectorMoveTests.out LLCellModelTests.out \
		HistogramTests.out WangLandauEnsembleTests.out SpeciesSwapMoveTests.out \
		EXEDOSEnsembleTests.out


# Google Test libs to link against.
GTEST_LIBS = $(GTEST_DIR)/lib/libgtest.a $(GTEST_DIR)/lib/libgtest_main.a

# House-keeping build targets.
all : pre-build $(TESTS)

pre-build:
	cd $(SRC_DIR) && $(MAKE)

clean :
	rm -f $(TESTS) $(BUILD_DIR)/*.o *.csv

$(BUILD_DIR)/%Tests.o: %Tests.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%Tests.out: $(BUILD_DIR)/%Tests.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ $(DEPS) -o $@
