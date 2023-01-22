#include <string>
#include <vector>

extern std::string content;
extern std::vector<unsigned int> line_indices;
extern unsigned int point;
extern unsigned int offset_line;
extern unsigned int offset_col;

unsigned int line_length(unsigned int index)
{
    if (index == line_indices.size() - 1) {
        return content.length() - line_indices[index];
    } else {
        return line_indices[index + 1] - line_indices[index];
    }
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
    if (offset_line > 0) {
        offset_line--;
    }
}

void scroll_down()
{
    offset_line++;
}

void scroll_left()
{
    if (offset_col > 0) {
        offset_col--;
    }
}

void scroll_right()
{
    offset_col++;
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
