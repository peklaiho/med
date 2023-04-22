#include "med.h"

#include <clocale>

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
        error("Give filenames as arguments");
    }

    std::vector<std::unique_ptr<Buffer>> buffers;
    int buffer_index = 0;

    for (int i = 1; i < argc; i++) {
        buffers.push_back(std::make_unique<Buffer>(argv[i]));
    }

    Screen screen;
    Keyboard keys;

    screen.draw(buffers[buffer_index].get());

    // Main loop
    while (true) {
        auto input = keys.process_input(screen, buffers[buffer_index].get());

        if (input == InputResult::exit_app) {
            break;
        } else if (input == InputResult::next_buffer) {
            if (buffer_index < static_cast<int>(buffers.size()) - 1) {
                buffer_index++;
            } else {
                buffer_index = 0;
            }
        } else if (input == InputResult::prev_buffer) {
            if (buffer_index > 0) {
                buffer_index--;
            } else {
                buffer_index = static_cast<int>(buffers.size()) - 1;
            }
        }

        screen.draw(buffers[buffer_index].get());
    }
}
