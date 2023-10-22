# Define the compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Define the target executable name
TARGET = csim

# Define the source and object files
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

# Default target
all: $(TARGET)

csim: $(TARGET)

# Rule to build the target from the object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files from source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean the object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

depend:
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak:
	touch $@

include depend.mak
