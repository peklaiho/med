#include <string>
#include <vector>

extern std::string content;
extern std::vector<int> line_indices;
extern int point;
extern int offset_line;
extern int offset_col;

extern int line_count();
extern int column_count();

// Return last index of current line
int line_end(int line)
{
    if (line == static_cast<int>(line_indices.size()) - 1) {
        return content.length();
    } else {
        return line_indices[line + 1] - 1;
    }
}

int current_line()
{
    int line = line_indices.size() - 1;

    while (true) {
        if (point < line_indices[line]) {
            line--;
        } else {
            return line;
        }
    }
}

int current_col()
{
    return point - line_indices[current_line()];
}

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

// Reconciliation

// After moving the point or changing the content of the buffer, it is possible
// the point is no longer visible on the screen.

// We must reconcile point and scroll offset so the point is visible again. We
// can reconcile either by moving point or offset.

// Defined below
void set_point(int value, bool reconcile);
void set_offset_line(int value, bool reconcile);
void set_offset_col(int value, bool reconcile);

void reconcile_by_moving_point()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = line_count() - 3;
    int last_buffer_col = column_count() - 1;

    if (line < offset_line) {
        set_point(line_indices[offset_line], false);
    }

    if (last_buffer_line >= 0 && line > (offset_line + last_buffer_line)) {
        set_point(line_indices[offset_line + last_buffer_line], false);
    }

    if (col < offset_col) {
        set_point(point + (offset_col - col), false);
    }

    if (last_buffer_col >= 0 && col > (offset_col + last_buffer_col)) {
        set_point(point - (col - (offset_col + last_buffer_col)), false);
    }
}

void reconcile_by_scrolling()
{
    int line = current_line();
    int col = current_col();

    int last_buffer_line = line_count() - 3;
    int last_buffer_col = column_count() - 1;

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

void set_point(int value, bool reconcile)
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
}

void set_offset_line(int value, bool reconcile)
{
    int max = line_indices.size() - 2;

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
    int max = line_end(current) - line_indices[current] - 2;

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

// Movement commands

void forward_character()
{
    set_point(point + 1, true);
}

void backward_character()
{
    set_point(point - 1, true);
}

void forward_word()
{

}

void backward_word()
{

}

void forward_paragraph()
{

}

void backward_paragraph()
{

}

void begin_of_line()
{
    set_point(line_indices[current_line()], true);
}

void end_of_line()
{
    set_point(line_end(current_line()), true);
}

void begin_of_buffer()
{
    set_point(0, true);
}

void end_of_buffer()
{
    set_point(content.length() - 1, true);
}

void forward_line()
{
    int current = current_line();

    if (current < (static_cast<int>(line_indices.size()) - 1)) {
        set_point(line_indices[current + 1], true);
    }
}

void backward_line()
{
    int current = current_line();

    if (current > 0) {
        set_point(line_indices[current - 1], true);
    }
}

void back_to_indentation()
{

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

}

void scroll_page_up()
{

}

void scroll_page_down()
{

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

}

void delete_word_backward()
{

}

void delete_rest_of_line()
{

}
