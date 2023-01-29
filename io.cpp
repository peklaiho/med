#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

bool file_error = false;

extern std::string filename;
extern std::string content;

extern void update_line_indices();

void error(const std::string txt)
{
    std::cerr << txt << '\n';
    exit(1);
}

void read_file()
{
    // Get the file size
    auto size = std::filesystem::file_size(filename);

    // Use binary mode because we want to transfer bytes exactly as they are
    auto file = std::ifstream(filename, std::ios_base::in | std::ios_base::binary);

    // Read file contents into memory
    content.resize(size);
    file.read(content.data(), size);

    // Make sure the number of bytes read matches the file size
    // https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html#es49-if-you-must-use-a-cast-use-a-named-cast
    if (static_cast<std::streamsize>(size) != file.gcount()) {
        error("Unable to read file");
    }
}

void write_file()
{
    auto file = std::ofstream(filename, std::ios_base::out | std::ios_base::binary);

    file.write(content.data(), content.size());

    if (file.fail()) {
        file_error = true;
    }
}

void init_io()
{
    if (std::filesystem::exists(filename)) {
        read_file();
    }

    update_line_indices();
}
