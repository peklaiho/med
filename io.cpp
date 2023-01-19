#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

extern std::string med_filename;
extern std::string med_content;

void med_error(const std::string error)
{
    std::cerr << error << "\n";
    exit(1);
}

void med_init_io()
{
    if (!std::filesystem::exists(med_filename)) {
        med_error("File does not exist");
    }

    auto size = std::filesystem::file_size(med_filename);
    auto file = std::ifstream(med_filename);

    med_content = std::string(size, 0);
    file.read(med_content.data(), size);

    if (size != file.gcount()) {
        med_error("Unable to read file");
    }
}

void med_write_file()
{

}
