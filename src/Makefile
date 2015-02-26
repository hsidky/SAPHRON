BUILD_DIR = ../build

BIN_DIR = ../bin

LIB_DIR = ../lib

# Flags passed to the C++ compiler.
CXXFLAGS += -static -Wall -Wextra -pthread -std=c++11

DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g3 -O3
else
	CXXFLAGS += -O3
endif

# Sources and Objects
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
SOURCES_LIB = $(filter-out main.cpp, $(SOURCES))
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))
OBJECTS_LIB = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES_LIB:.cpp=.o)))

# Target application name
TARGET = SAPHRON
TARGET_LIB = SAPHRON.a

LINKER_DEPS = -llapack -lblas -larpack -lgfortran -lquadmath

# House-keeping build targets.
all : $(OBJECTS)  $(OBJECTS_LIB) $(TARGET) $(TARGET_LIB)

install: $(TARGET)
	install $(BIN_DIR)/$(TARGET) /usr/local/bin

clean :
	rm -f $(BIN_DIR)/* $(BUILD_DIR)/*.o $(LIB_DIR)/*.a

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Ensembles/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: DataLoggers/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Models/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Moves/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Simulation/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Visitors/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS) $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $(BIN_DIR)/$@ $(LINKER_DEPS)

$(TARGET_LIB): $(OBJECTS_LIB)
	ar rc $(LIB_DIR)/$@ $(OBJECTS_LIB)
