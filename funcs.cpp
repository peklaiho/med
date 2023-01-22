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

// Editing commands
void insert_character(char c)
{
    content.insert(point, 1, c);
    point++;
}
