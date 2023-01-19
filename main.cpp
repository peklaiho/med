#include <clocale>
#include <string>

std::string med_filename;
std::string med_content;

extern void med_destroy_ui();
extern void med_error(const std::string error);
extern int med_get_key();
extern void med_init_io();
extern void med_init_ui();

int main(int argc, char *argv[])
{
    if (argc < 2) {
        med_error("Give filename as argument");
    }

    med_filename = argv[1];

    // Use locale from environment
    if (!setlocale(LC_ALL, "")) {
        med_error("Unable to set locale");
    }

    // Inits
    med_init_io();
    med_init_ui();

    med_error(med_content);

    int key = med_get_key();

    med_destroy_ui();
    return 0;
}
