#include "med.h"

extern std::string content;
extern std::vector<line_index_pair> line_indices;

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
