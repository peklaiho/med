# Use this website for more info about Makefiles
# https://makefiletutorial.com/

# Variables
CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -std=c++20
LDFLAGS = -O2 -Wall -Wextra -std=c++20

# Link the object files into an executable
# The variable $^ is replaced with all the dependencies
med: buffer.o key.o main.o ui.o
	$(CXX) $(LDFLAGS) $^ -o $@ -lncurses

# Link the object files into an executable
# The variable $^ is replaced with all the dependencies
med-utf8: buffer.o key.o main.o ui.o utf8.o
	$(CXX) $(LDFLAGS) -DMED_UTF8 $^ -o $@ -lncursesw

# Compile individual .cpp files into .o object files
# The variable $< is replaced with the first dependency
%.o: %.cpp med.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Install the app in ~/bin
install: med
	cp -f med ~/bin/med

# Delete the executable and object files
clean:
	rm -f med med-utf8 *.o

# Phone targets
.PHONY: clean install
