#include <clocale>
#include "med.h"

// Global variables
std::string filename;
std::string content;
std::vector<line_index_pair> line_indices;
int point = 0;
bool redraw_screen = false;

// External functions
extern void destroy_ui();
extern void draw_screen();
extern void error(const std::string txt);
extern int get_key();
extern void init_io();
extern void init_ui();

void main_loop()
{
    int key = get_key();
    draw_screen();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        error("Give filename as argument");
    }

    filename = argv[1];

    // Use locale from environment
    if (!setlocale(LC_ALL, "")) {
        error("Unable to set locale");
    }

    // Inits
    init_io();
    init_ui();

    // Main loop
    main_loop();

    // Cleanup
    destroy_ui();
    return 0;
}
