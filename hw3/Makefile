# Define the compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Define the target executable name
TARGET = csim

# Define the source files
SRCS = csim.cpp

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Rule to build the target from the object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to clean the object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

depend :
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak