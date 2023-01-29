# Use this website for more info about Makefiles
# https://makefiletutorial.com/

# Variables
CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -std=c++20
LDFLAGS = -O2 -Wall -Wextra -std=c++20

# Link the object files into an executable
# The variable $^ is replaced with all the dependencies
med: funcs.o io.o key.o main.o ui.o
	$(CXX) $(LDFLAGS) -lncurses $^ -o $@

# Compile individual .cpp files into .o object files
# The variable $< is replaced with the first dependency
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Install the app in ~/bin
install: med
	cp -f med ~/bin/med

# Delete the executable and object files
clean:
	rm -f med *.o
