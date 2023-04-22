#include "med.h"

#include <clocale>

// Global variables
bool exit_app = false;
std::unique_ptr<Buffer> current_buffer;

// External functions
extern int get_screen_width();
extern int get_screen_height();
extern void destroy_ui();
extern void draw_screen();
extern void init_io();
extern void init_ui();
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

    // Init ui
    init_ui();

    current_buffer = std::make_unique<Buffer>(argv[1]);
    current_buffer->set_screen_size(get_screen_width(), get_screen_height());

    draw_screen();

    // Main loop
    while (!exit_app) {
        process_input();
        draw_screen();
    }

    // Cleanup UI
    destroy_ui();
}
