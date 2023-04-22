#include "med.h"

#include <clocale>

// Global variables
bool exit_app = false;
std::unique_ptr<Buffer> current_buffer;

// External functions
extern void process_input();

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

    current_buffer = std::make_unique<Buffer>(argv[1]);
    Screen screen;

    screen.draw(*current_buffer);

    // Main loop
    while (!exit_app) {
        process_input();
        screen.draw(*current_buffer);
    }
}
