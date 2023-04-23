#include "med.h"

#include <clocale>

bool show_prompt = false;

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

    std::vector<Buffer> buffers;
    int buffer_index = 0;

    for (int i = 1; i < argc; i++) {
        // emplace_back constructs object in-place and appends
        // it to the vector, avoiding copy or move operation
        buffers.emplace_back(argv[i]);
    }

    Screen screen;
    Keyboard keys;

    // Main loop
    while (true) {
        screen.draw(buffers[buffer_index]);

        if (show_prompt) {
            bool quit_app = true;

            for (int i = 0; i < static_cast<int>(buffers.size()); ) {
                if (buffers[i].get_content_changed()) {
                    screen.draw(buffers[i]);

                    auto input = keys.read_prompt();

                    if (input == InputResult::none) {
                        // Invalid input, do nothing
                    } else if (input == InputResult::screen_size) {
                        // Screen size: do not go to next
                        screen.size_changed();
                    } else if (input == InputResult::prompt_no) {
                        // No: go to next
                        i++;
                    } else if (input == InputResult::prompt_yes) {
                        // Yes: save and go to next
                        buffers[i].write_file();
                        i++;
                    } else if (input == InputResult::prompt_quit) {
                        // Cancel quit
                        quit_app = false;
                        break;
                    }
                } else {
                    // Not changed: go to next
                    i++;
                }
            }

            if (quit_app) {
                break;
            } else {
                show_prompt = false;
            }
        } else {
            auto input = keys.read_input(buffers[buffer_index]);

            if (input == InputResult::screen_size) {
                screen.size_changed();
            } else if (input == InputResult::exit_app) {
                show_prompt = true;
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
        }
    }
}
