#include <string>
#include <vector>

// Global variables
std::vector<int> line_indices;
std::string content;
int point = 0;
int offset_line = 0;
int offset_col = 0;
int goal_col = 0;
bool edit_mode = false;

extern int screen_height();
extern int screen_width();

constexpr int min_max(int val, int min, int max)
{
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

// Update new indexes of lines
// Must be called always after content changes!
void update_line_indices()
{
    line_indices.clear();

    line_indices.push_back(0);

    for (int i = 0; i < static_cast<int>(content.size()); i++) {
        if (content[i] == '\n') {
            line_indices.push_back(i + 1);
        }
    }
}

int num_of_lines()
{
    return line_indices.size();
}

// Return first index of given line
int line_start(int line)
{
    return line_indices[line];
}

// Return last index of given line
int line_end(int line)
{
    if (line == num_of_lines() - 1) {
        return content.length();
    } else {
        return line_start(line + 1) - 1;
    }
}

int current_line()
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

int current_col()
{
    return point - line_start(current_line());
}

// Reconciliation

// After moving the point or changing the content of the buffer, it is possible
// the point is no longer visible on the screen.

// We must reconcile point and scroll offset so the point is visible again. We
// can reconcile either by moving point or offset.

// Defined below
void set_point(int value, bool reconcile, bool set_goal);
bool goto_line(int line, bool reconcile);
void set_offset_line(int value, bool reconcile);
void set_offset_col(int value, bool reconcile);

void reconcile_by_moving_point()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = screen_height() - 3;
    int last_buffer_col = screen_width() - 1;

    if (line < offset_line) {
        goto_line(offset_line, false);
    }

    if (last_buffer_line >= 0 && line > (offset_line + last_buffer_line)) {
        goto_line(offset_line + last_buffer_line, false);
    }

    if (col < offset_col) {
        set_point(point + (offset_col - col), false, true);
    }

    if (last_buffer_col >= 0 && col > (offset_col + last_buffer_col)) {
        set_point(point - (col - (offset_col + last_buffer_col)), false, true);
    }
}

void reconcile_by_scrolling()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = screen_height() - 3;
    int last_buffer_col = screen_width() - 1;

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

// Setters that call reconciliation as needed

void set_point(int value, bool reconcile, bool set_goal)
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

bool goto_line(int line, bool reconcile)
{
    if (line >= 0 && line < num_of_lines()) {
        set_point(min_max(line_start(line) + goal_col, line_start(line), line_end(line)), reconcile, false);
        return true;
    }

    return false;
}

void set_offset_line(int value, bool reconcile)
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

void set_offset_col(int value, bool reconcile)
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

// Character helper
constexpr bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Searching

int word_boundary_forward(int index)
{
    for (; index < static_cast<int>(content.length()) - 1; index++) {
        if (!is_whitespace(content[index]) && is_whitespace(content[index + 1])) {
            return index + 1;
        }
    }

    return -1;
}

int word_boundary_backward(int index)
{
    for (; index > 0; index--) {
        if (!is_whitespace(content[index]) && is_whitespace(content[index - 1])) {
            return index;
        }
    }

    return -1;
}

int paragraph_boundary_forward(int index)
{
    for (; index < static_cast<int>(content.length()) - 1; index++) {
        if (content[index] == '\n' && content[index + 1] == '\n') {
            return index + 1;
        }
    }

    return -1;
}

int paragraph_boundary_backward(int index)
{
    for (; index > 0; index--) {
        if (content[index] == '\n' && content[index - 1] == '\n') {
            return index;
        }
    }

    return -1;
}

// Movement commands

void begin_of_buffer()
{
    set_point(0, true, true);
}

void end_of_buffer()
{
    set_point(content.length(), true, true);
}

void forward_character()
{
    set_point(point + 1, true, true);
}

void backward_character()
{
    set_point(point - 1, true, true);
}

void forward_word()
{
    int result = word_boundary_forward(point);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        end_of_buffer();
    }
}

void backward_word()
{
    int result = word_boundary_backward(point - 1);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        begin_of_buffer();
    }
}

void forward_paragraph()
{
    int result = paragraph_boundary_forward(point);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        end_of_buffer();
    }
}

void backward_paragraph()
{
    int result = paragraph_boundary_backward(point - 1);

    if (result >= 0) {
        set_point(result, true, true);
    } else {
        begin_of_buffer();
    }
}

void begin_of_line()
{
    set_point(line_start(current_line()), true, true);
}

void end_of_line()
{
    set_point(line_end(current_line()), true, true);
}

void forward_line()
{
    goto_line(current_line() + 1, true);
}

void backward_line()
{
    goto_line(current_line() - 1, true);
}

void back_to_indentation()
{
    int current = current_line();
    int i = line_start(current);

    while (i < line_end(current) && (content[i] == ' ' || content[i] == '\t')) {
        i++;
    }

    set_point(i, true, true);
}

// Scrolling

void scroll_up()
{
    set_offset_line(offset_line - 1, true);
}

void scroll_down()
{
    set_offset_line(offset_line + 1, true);
}

void scroll_left()
{
    set_offset_col(offset_col - 1, true);
}

void scroll_right()
{
    set_offset_col(offset_col + 1, true);
}

void scroll_current_line_middle()
{
    set_offset_line(current_line() - ((screen_height() - 2) / 2), true);
}

void scroll_page_up()
{
    set_offset_line(offset_line - screen_height() + 3, true);
}

void scroll_page_down()
{
    set_offset_line(offset_line + screen_height() - 3, true);
}

// Editing: insertion

void insert_character(char c)
{
    content.insert(point, 1, c);
    update_line_indices();
    forward_character();
}

// Editing: deletion

void delete_character_forward()
{
    if (point < static_cast<int>(content.length())) {
        content.erase(point, 1);
        update_line_indices();
    }
}

void delete_character_backward()
{
    if (point > 0) {
        content.erase(point - 1, 1);
        update_line_indices();
        backward_character();
    }
}

void delete_word_forward()
{
    int len = static_cast<int>(content.length());

    if (point < len) {
        int result = word_boundary_forward(point);

        if (result >= 0) {
            content.erase(point, result - point);
            update_line_indices();
        } else {
            content.erase(point, len - point);
            update_line_indices();
        }
    }
}

void delete_word_backward()
{
    if (point > 0) {
        int result = word_boundary_backward(point - 1);

        if (result >= 0) {
            content.erase(result, point - result);
            update_line_indices();
            set_point(result, true, true);
        } else {
            content.erase(0, point);
            update_line_indices();
            begin_of_buffer();
        }
    }
}

void delete_rest_of_line()
{
    int end = line_end(current_line());

    if (point < end) {
        content.erase(point, end - point);
        update_line_indices();
    } else {
        // Delete one character (the newline)
        delete_character_forward();
    }
}
