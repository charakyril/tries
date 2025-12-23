# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 

# Target executable
TARGET = oop

# Source files
SRC = OOP-PROJECT.cpp world.cpp


# Object files
OBJ = $(SRC:.cpp=.o)

.PHONY: all clean run

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)
# Compile source files into object files
%.o: %.cpp types.h world.h sensor.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)
# Run the tests
run: $(TARGET)
	./$(TARGET)
