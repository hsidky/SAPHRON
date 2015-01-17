BUILD_DIR = ../build

BIN_DIR = ../bin

CXX = clang++

# Flags passed to the C++ compiler.
CXXFLAGS += -Wall -Wextra -pthread -std=c++11 -O3

# Sources and Objects
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))

# Target application name
TARGET = LNTMC

# House-keeping build targets.
all : $(OBJECTS) $(TARGET)

test:
	@echo $(SOURCES)

install: $(TARGET)
	install $(BIN_DIR)/$(TARGET) /usr/local/bin

clean :
	rm -f $(BIN_DIR)/* $(BUILD_DIR)/*.o

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
