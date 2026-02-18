# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Target executable
TARGET = loadbalancer

# Source files
SRCS = main.cpp request.cpp webServer.cpp loadBalancer.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
DEPS = request.h webServer.h loadBalancer.h

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile step with dependency tracking
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild everything
rebuild: clean all
