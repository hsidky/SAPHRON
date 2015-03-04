# Root of gtest dir.
GTEST_DIR = ../gtest

BUILD_DIR = ../build

LIB_DIR  = ../lib

SRC_DIR = ../src

# Flags passed to the preprocessor.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++11 -O3 -include ../include/gcc-preinclude.h

# Build dependencies
DEPS  = $(LIB_DIR)/SAPHRON.a

# All tests produced by this Makefile.
TESTS = RandTests.out SiteTests.out BaseModelTests.out Ising3DModelTests.out \
	    FlipSpinMoveTests.out NVTEnsembleTests.out \
		LebwohlLasherModelTests.out SphereUnitVectorMoveTests.out LLCellModelTests.out \
		HistogramTests.out WangLandauDOSEnsembleTests.out SpeciesSwapMoveTests.out \
		SemiGrandDOSEnsembleTests.out ParallelDOSEnsembleTests.out SimFlagsTests.out


# Google Test libs to link against.
GTEST_LIBS = $(GTEST_DIR)/lib/libgtest.a $(GTEST_DIR)/lib/libgtest_main.a

LINKER_DEPS = -llapack -lblas -lgfortran -lquadmath


# House-keeping build targets.
all : pre-build $(TESTS)

pre-build:
	cd $(SRC_DIR) && $(MAKE)

clean :
	rm -f $(TESTS) $(BUILD_DIR)/*Tests.o *.csv

$(BUILD_DIR)/%Tests.o: %Tests.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%Tests.out: $(BUILD_DIR)/%Tests.o $(GTEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ $(DEPS) -o $@ $(LINKER_DEPS)
