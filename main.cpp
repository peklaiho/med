#include <string>
#include <vector>
#include <clocale>

// Global variables
std::string med_version = "0.1";
std::string filename;
std::string content;
std::vector<unsigned int> line_indices;
unsigned int point = 0;
unsigned int offset_line = 0;
unsigned int offset_col = 0;
bool edit_mode = false;
bool exit_app = false;

// External functions
extern void destroy_ui();
extern void draw_screen();
extern void debug(const std::string txt);
extern void error(const std::string txt);
extern void init_io();
extern void init_ui();
extern void process_input();

void main_loop()
{
    while (!exit_app) {
        process_input();
        draw_screen();
    }
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
