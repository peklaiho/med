CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -std=c++20
LDFLAGS = -O2 -Wall -Wextra -std=c++20

med: main.o io.o ui.o
	$(CXX) $(LDFLAGS) -lncurses $^ -o $@

main.o: main.cpp

io.o: io.cpp

ui.o: ui.cpp

clean:
	rm -f med *.o
