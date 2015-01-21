BUILD_DIR = ../build

BIN_DIR = ../bin

LIB_DIR = ../lib

CXX = clang++

# Flags passed to the C++ compiler.
CXXFLAGS += -Wall -Wextra -pthread -std=c++11 -O3

# Sources and Objects
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
SOURCES_LIB = $(filter-out main.cpp, $(SOURCES))
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))
OBJECTS_LIB = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES_LIB:.cpp=.o)))

# Target application name
TARGET = LNTMC
TARGET_LIB = LNTMC.a

# House-keeping build targets.
all : $(OBJECTS)  $(OBJECTS_LIB) $(TARGET) $(TARGET_LIB)

test:
	@echo $(SOURCES)

install: $(TARGET)
	install $(BIN_DIR)/$(TARGET) /usr/local/bin

clean :
	rm -f $(BIN_DIR)/* $(BUILD_DIR)/*.o $(LIB_DIR)/*.a

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Ensembles/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Loggers/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Models/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: Moves/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $(BIN_DIR)/$@

$(TARGET_LIB): $(OBJECTS_LIB)
	ar rc $(LIB_DIR)/$@ $<
