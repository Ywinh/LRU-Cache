# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -g -Iinclude -Ibuild/install/include/libcuckoo

# Linker flags
LDFLAGS = -L/usr/local/lib -lbenchmark -lpthread

# Source and include directories
INCLUDE_DIR = include
SRC_DIR = src

# Source files
SRCS =  bigtest.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = out

# Default rule to build the executable
all: $(TARGET)


# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
