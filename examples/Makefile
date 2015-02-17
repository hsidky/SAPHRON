BUILD_DIR = ../build

LIB_DIR  = ../lib

SRC_DIR = ../src

# Flags passed to the C++ compiler.
CXXFLAGS += -Wall -Wextra -pthread -std=c++11

DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g3 -O3
else
	CXXFLAGS += -O3
endif

# Build dependencies
DEPS  = $(LIB_DIR)/SAPHRON.a

# All tests produced by this Makefile.
EXAMPLES = lebwohl_lasher_nvt.out ll_cell_twist_nvt.out wang_landau_ll.out \
		   ll_binary_mix_exedos.out ll_binary_mix_exedos_extended.out

# House-keeping build targets.
all : pre-build $(EXAMPLES)

pre-build:
	cd $(SRC_DIR) && $(MAKE) DEBUG=$(DEBUG)

clean :
	rm -f $(EXAMPLES) $(BUILD_DIR)/*.o

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.out: $(BUILD_DIR)/%.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ $(DEPS) -o $@
