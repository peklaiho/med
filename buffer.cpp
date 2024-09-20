#include "med.h"

#include <fstream>
#include <filesystem>

extern void error(std::string_view txt);

// ---------------
// Private methods
// ---------------

// Update new indexes of lines
// Must be called always after content changes!
void Buffer::update_line_indices()
{
    line_indices.clear();

    line_indices.push_back(0);

    for (int i = 0; i < static_cast<int>(content.size()); i++) {
        if (content[i] == '\n') {
            line_indices.push_back(i + 1);
        }
    }
}

// Setters that call reconcialition as needed

void Buffer::set_point(int value, bool reconcile, bool set_goal)
{
    if (value > static_cast<int>(content.length())) {
        value = content.length();
    }
    if (value < 0) {
        value = 0;
    }

    point = value;

    if (reconcile) {
        reconcile_by_scrolling();
    }

    if (set_goal) {
        goal_col = current_col();
    }
}

bool Buffer::set_line(int line, bool reconcile)
{
    if (line < 0 || line >= num_of_lines()) {
        return false;
    }

    int min = line_start(line);
    int max = line_end(line);
    int p = min + goal_col;

    if (p < min) {
        p = min;
    }
    if (p > max) {
        p = max;
    }

    set_point(p, reconcile, false);

    return true;
}

void Buffer::set_offset_line(int value, bool reconcile)
{
    int max = num_of_lines() - 2;

    if (value > max) {
        value = max;
    }
    if (value < 0) {
        value = 0;
    }

    offset_line = value;

    if (reconcile) {
        reconcile_by_moving_point();
    }
}

void Buffer::set_offset_col(int value, bool reconcile)
{
    int current = current_line();
    int max = line_end(current) - line_start(current) - 2;

    if (value > max) {
        value = max;
    }
    if (value < 0) {
        value = 0;
    }

    offset_col = value;

    if (reconcile) {
        reconcile_by_moving_point();
    }
}

// Reconciliation

// After moving the point or changing the content of the buffer, it is possible
// the point is no longer visible on the screen.

// We must reconcile point and scroll offset so the point is visible again. We
// can reconcile either by moving point or offset.

void Buffer::reconcile_by_moving_point()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = screen_height - 3;
    int last_buffer_col = screen_width - 1;

    if (line < offset_line) {
        set_line(offset_line, false);
    }

    if (last_buffer_line >= 0 && line > (offset_line + last_buffer_line)) {
        set_line(offset_line + last_buffer_line, false);
    }

    if (col < offset_col) {
        set_point(point + (offset_col - col), false, true);
    }

    if (last_buffer_col >= 0 && col > (offset_col + last_buffer_col)) {
        set_point(point - (col - (offset_col + last_buffer_col)), false, true);
    }
}

void Buffer::reconcile_by_scrolling()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = screen_height - 3;
    int last_buffer_col = screen_width - 1;

    if (line < offset_line) {
        set_offset_line(line, false);
    }

    if (last_buffer_line >= 0 && line > (offset_line + last_buffer_line)) {
        set_offset_line(line - last_buffer_line, false);
    }

    if (col < offset_col) {
        set_offset_col(col, false);
    }

    if (last_buffer_col >= 0 && col > (offset_col + last_buffer_col)) {
        set_offset_col(col - last_buffer_col, false);
    }
}

// Character helpers

constexpr bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

constexpr bool is_letter_or_digit(char c)
{
    return (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

// Search helpers

int Buffer::word_boundary_forward(int index) const
{
    for (; index < static_cast<int>(content.length()) - 1; index++) {
        if (is_letter_or_digit(content[index]) && !is_letter_or_digit(content[index + 1])) {
            return index + 1;
        }
    }

    return -1;
}

int Buffer::word_boundary_backward(int index) const
{
    for (; index > 0; index--) {
        if (is_letter_or_digit(content[index]) && !is_letter_or_digit(content[index - 1])) {
            return index;
        }
    }

    return -1;
}

int Buffer::paragraph_boundary_forward(int index) const
{
    for (; index < static_cast<int>(content.length()) - 1; index++) {
        if (content[index] == '\n' && content[index + 1] == '\n') {
            return index + 1;
        }
    }

    return -1;
}

int Buffer::paragraph_boundary_backward(int index) const
{
    for (; index > 0; index--) {
        if (content[index] == '\n' && content[index - 1] == '\n') {
            return index;
        }
    }

    return -1;
}

// --------------
// Public methods
// --------------

// Constructor
Buffer::Buffer(std::string fname)
{
    filename = fname;

    if (std::filesystem::exists(filename)) {
        read_file();
    } else {
        update_line_indices();
    }
}

// I/O

void Buffer::read_file()
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

    update_line_indices();
}

void Buffer::write_file()
{
    auto file = std::ofstream(filename, std::ios_base::out | std::ios_base::binary);

    file.write(content.data(), content.size());

    if (file.fail()) {
        error("Unable to write file");
    }
}

// Getters

std::string Buffer::get_filename() const
{
    return filename;
}

std::string_view Buffer::get_content() const
{
    return std::string_view { content };
}

int Buffer::get_point() const
{
    return point;
}

int Buffer::num_of_lines() const
{
    return line_indices.size();
}

// Return first index of given line
int Buffer::line_start(int line) const
{
    return line_indices[line];
}

// Return last index of given line
int Buffer::line_end(int line) const
{
    if (line == num_of_lines() - 1) {
        return content.length();
    } else {
        return line_start(line + 1) - 1;
    }
}

int Buffer::current_line() const
{
    int line = num_of_lines() - 1;

    while (true) {
        if (point < line_start(line)) {
            line--;
        } else {
            return line;
        }
    }
}

int Buffer::current_col() const
{
    return point - line_start(current_line());
}

int Buffer::get_offset_line() const
{
    return offset_line;
}

int Buffer::get_offset_col() const
{
    return offset_col;
}

bool Buffer::get_edit_mode() const
{
    return edit_mode;
}

bool Buffer::get_content_changed() const
{
    return content_changed;
}

// Setters

void Buffer::set_screen_size(int width, int height)
{
    if (screen_width != width || screen_height != height) {
        screen_width = width;
        screen_height = height;
        reconcile_by_scrolling();
    }
}

void Buffer::set_edit_mode(bool value)
{
    edit_mode = value;
}

void Buffer::store_point_location()
{
    previous_point = point;
}

void Buffer::restore_point_location()
{
    set_point(previous_point, true, true);
}

// Movement commands

void Buffer::begin_of_buffer()
{
    set_point(0, true, true);
}

void Buffer::end_of_buffer()
{
    set_point(content.length(), true, true);
}

void Buffer::forward_character()
{
    set_point(point + 1, true, true);
}

void Buffer::backward_character()
{
    set_point(point - 1, true, true);
}

void Buffer::forward_word()
{
    int result = word_boundary_forward(point);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        end_of_buffer();
    }
}

void Buffer::backward_word()
{
    int result = word_boundary_backward(point - 1);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        begin_of_buffer();
    }
}

void Buffer::forward_paragraph()
{
    int result = paragraph_boundary_forward(point);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        end_of_buffer();
    }
}

void Buffer::backward_paragraph()
{
    int result = paragraph_boundary_backward(point - 1);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        begin_of_buffer();
    }
}

void Buffer::begin_of_line()
{
    set_point(line_start(current_line()), true, true);
}

void Buffer::end_of_line()
{
    set_point(line_end(current_line()), true, true);
}

void Buffer::forward_line()
{
    set_line(current_line() + 1, true);
}

void Buffer::backward_line()
{
    set_line(current_line() - 1, true);
}

void Buffer::back_to_indentation()
{
    int current = current_line();
    int i = line_start(current);

    while (i < line_end(current) && (content[i] == ' ' || content[i] == '\t')) {
        i++;
    }

    set_point(i, true, true);
}

void Buffer::goto_line(int line)
{
    if (line < 0) {
        line = 0;
    } else if (line >= num_of_lines()) {
        line = num_of_lines() - 1;
    }

    set_line(line, true);

    scroll_current_line_middle();
}

// Scrolling

void Buffer::scroll_up()
{
    set_offset_line(offset_line - 1, true);
}

void Buffer::scroll_down()
{
    set_offset_line(offset_line + 1, true);
}

void Buffer::scroll_left()
{
    set_offset_col(offset_col - 1, true);
}

void Buffer::scroll_right()
{
    set_offset_col(offset_col + 1, true);
}

void Buffer::scroll_current_line_middle()
{
    set_offset_line(current_line() - ((screen_height - 2) / 2), true);
}

void Buffer::scroll_page_up()
{
    set_offset_line(offset_line - screen_height + 3, true);
}

void Buffer::scroll_page_down()
{
    set_offset_line(offset_line + screen_height - 3, true);
}

// Editing: insertion

void Buffer::insert_character(char c)
{
    content.insert(point, 1, c);
    content_changed = true;
    update_line_indices();

    forward_character();
}

// Editing: deletion

void Buffer::delete_character_forward()
{
    if (point < static_cast<int>(content.length())) {
        content.erase(point, 1);
        content_changed = true;
        update_line_indices();
    }
}

void Buffer::delete_character_backward()
{
    if (point > 0) {
        content.erase(point - 1, 1);
        content_changed = true;
        update_line_indices();

        backward_character();
    }
}

void Buffer::delete_word_forward()
{
    int len = static_cast<int>(content.length());

    if (point < len) {
        int result = word_boundary_forward(point);

        if (result >= 0) {
            content.erase(point, result - point);
            content_changed = true;
            update_line_indices();
        } else {
            content.erase(point, len - point);
            content_changed = true;
            update_line_indices();
        }
    }
}

void Buffer::delete_word_backward()
{
    if (point > 0) {
        int result = word_boundary_backward(point - 1);

        if (result >= 0) {
            content.erase(result, point - result);
            content_changed = true;
            update_line_indices();

            set_point(result, true, true);
        } else {
            content.erase(0, point);
            content_changed = true;
            update_line_indices();

            begin_of_buffer();
        }
    }
}

void Buffer::delete_rest_of_line()
{
    int end = line_end(current_line());

    if (point < end) {
        content.erase(point, end - point);
        content_changed = true;
        update_line_indices();
    } else {
        // Delete one character (the newline)
        delete_character_forward();
    }
}

// Searching

bool Buffer::search_forward(std::string_view txt)
{
    if (point == static_cast<int>(content.length())) {
        return false;
    }

    auto pos = content.find(txt, point + 1);

    if (pos == std::string::npos) {
        return false;
    }

    set_point(pos, true, true);
    return true;
}

bool Buffer::search_backward(std::string_view txt)
{
    if (point == 0) {
        return false;
    }

    auto pos = content.rfind(txt, point - 1);

    if (pos == std::string::npos) {
        return false;
    }

    set_point(pos, true, true);
    return true;
}
