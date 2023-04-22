#include "med.h"

#include <clocale>

// Global variables
bool exit_app = false;

void error(std::string_view txt)
{
    std::cerr << txt << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    // Use locale from environment
    if (!setlocale(LC_ALL, "")) {
        error("Unable to set locale");
    }

    if (argc < 2) {
        error("Give filename as argument");
    }

    Buffer buffer { argv[1] };
    Screen screen;
    Keyboard keys;

    screen.draw(buffer);

    // Main loop
    while (!exit_app) {
        keys.process_input(screen, buffer);
        screen.draw(buffer);
    }
}
