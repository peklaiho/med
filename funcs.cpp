#include "med.h"

extern std::string content;
extern std::vector<line_index_pair> line_indices;
extern cursor_position point;

void update_line_indices()
{
    line_indices.clear();

    line_index start = 0;
    line_index end = 0;

    for (; end < content.size(); end++) {
        if (content[end] == newline) {
            line_indices.push_back(make_line_index(start, end));
            start = end + 1;
        }
    }

    line_indices.push_back(make_line_index(start, end));
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

// Movement commands

void forward_character()
{

}

void backward_character()
{

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

}

void end_of_line()
{

}

void begin_of_buffer()
{

}

void end_of_buffer()
{

}

void forward_line()
{

}

void backward_line()
{

}

void back_to_indentation()
{

}

// Scrolling

void scroll_up()
{

}

void scroll_down()
{

}

void scroll_left()
{

}

void scroll_right()
{

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
    point++;
}

// Editing: deletion

void delete_character_forward()
{

}

void delete_character_backward()
{

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
