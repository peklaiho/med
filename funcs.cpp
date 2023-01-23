#include <string>
#include <vector>

extern std::string content;
extern std::vector<unsigned int> line_indices;
extern unsigned int point;
extern unsigned int offset_line;
extern unsigned int offset_col;

extern void debug(const std::string txt);
extern void error(const std::string txt);

unsigned int line_length(unsigned int index)
{
    if (index >= line_indices.size()) {
        error("Attempt to get line length for invalid index");
    }

    if (index == line_indices.size() - 1) {
        return content.length() - line_indices[index];
    } else {
        return line_indices[index + 1] - line_indices[index];
    }
}

unsigned int current_line()
{
    unsigned int line = line_indices.size() - 1;

    while (true) {
        if (point < line_indices[line]) {
            line--;
        } else {
            return line;
        }
    }
}

unsigned int current_col()
{
    return point - line_indices[current_line()];
}

void update_line_indices()
{
    line_indices.clear();

    line_indices.push_back(0);

    for (unsigned int i = 0; i < content.size(); i++) {
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

void reconcile_by_moving_point()
{

}

void reconcile_by_scrolling()
{

}

// Setters that call reconciliation as needed

void set_point(int value)
{
    if (value < 0) {
        value = 0;
    }
    if (value > static_cast<int>(content.length())) {
        value = content.length();
    }

    point = static_cast<unsigned int>(value);

    reconcile_by_scrolling();
}

void set_offset_line(int value)
{
    int max = line_indices.size() - 2;

    if (value > max) {
        value = max;
    }
    if (value < 0) {
        value = 0;
    }

    offset_line = value;
    reconcile_by_moving_point();
}

void set_offset_col(int value)
{
    int max = line_length(current_line()) - 2;

    if (value > max) {
        value = max;
    }
    if (value < 0) {
        value = 0;
    }

    offset_col = value;
    reconcile_by_moving_point();
}

// Movement commands

void forward_character()
{
    set_point(point + 1);
}

void backward_character()
{
    set_point(point - 1);
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
    auto current = current_line();
    set_point(line_indices[current]);
}

void end_of_line()
{
    auto current = current_line();
    auto len = line_length(current);
    set_point(line_indices[current] + len - 1);
}

void begin_of_buffer()
{
    set_point(0);
}

void end_of_buffer()
{
    set_point(content.length() - 1);
}

void forward_line()
{
    auto current = current_line();
    auto len = line_length(current);
    set_point(line_indices[current] + len);
}

void backward_line()
{
    auto current = current_line();
    set_point(line_indices[current] - 1);
}

void back_to_indentation()
{

}

// Scrolling

void scroll_up()
{
    set_offset_line(offset_line - 1);
}

void scroll_down()
{
    set_offset_line(offset_line + 1);
}

void scroll_left()
{
    set_offset_col(offset_col - 1);
}

void scroll_right()
{
    set_offset_col(offset_col + 1);
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
    if (point < content.length()) {
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
