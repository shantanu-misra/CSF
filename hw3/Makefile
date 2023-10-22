# Define the compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Define the target executable name
TARGET = csim

# Define directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Define the source and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# Default target
all: $(BINDIR)/$(TARGET)

csim: $(BINDIR)/$(TARGET)

# Rule to build the target from the object files
$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean the object files and the executable
clean:
	rm -f $(OBJS) $(BINDIR)/$(TARGET)

depend:
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak:
	touch $@

include depend.mak
