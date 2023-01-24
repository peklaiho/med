#include <string>
#include <vector>
#include <ncurses.h>

extern std::string med_version;
extern std::string filename;
extern std::string content;
extern std::vector<unsigned int> line_indices;
extern unsigned int point;
extern unsigned int offset_line;
extern unsigned int offset_col;
extern bool edit_mode;

extern void error(const std::string txt);
extern unsigned int line_end(unsigned int index);
extern unsigned int current_line();
extern unsigned int current_col();

bool redraw_screen = false;

unsigned int line_count()
{
    return static_cast<unsigned int>(LINES);
}

unsigned int column_count()
{
    return static_cast<unsigned int>(COLS);
}

void draw_buffer()
{
    color_set(0, 0);

    for (unsigned int row = 0; row < (line_count() - 2); row++) {
        unsigned int line = row + offset_line;
        if (line < line_indices.size()) {
            unsigned int start = line_indices[line] + offset_col;
            unsigned int end = line_end(line);
            int len = end - start;
            if (len > 0) {
                mvaddnstr(row, 0, content.data() + start, len);
            }
        }
    }
}

void draw_statusbar()
{
    color_set(1, 0);

    std::string str(column_count(), ' ');
    int col = 2;

    // Mode
    if (edit_mode) {
        str.replace(col, 4, "EDIT");
        col += 6;
    }

    // Line and column
    std::string temp = std::to_string(current_line() + 1);
    str.replace(col, temp.length(), temp);
    col += temp.length();
    str.replace(col, 1, ":");
    col++;
    temp = std::to_string(current_col());
    str.replace(col, temp.length(), temp);
    col += temp.length() + 2;

    // Debug
    temp = std::to_string(point);
    str.replace(col, temp.length(), temp);
    col += temp.length();
    str.replace(col, 1, ":");
    col++;
    temp = std::to_string(content.length());
    str.replace(col, temp.length(), temp);
    col += temp.length() + 2;

    // Filename
    str.replace(col, filename.length(), filename);
    col += filename.length();

    mvaddnstr(line_count() - 2, 0, str.data(), column_count());
}

void draw_minibuffer()
{
    color_set(0, 0);
}

void draw_cursor()
{
    auto current = current_line();
    auto col = current_col();

    move(current - offset_line, col - offset_col);
}

void draw_screen()
{
    if (redraw_screen) {
        clear();
    } else {
        erase();
    }

    redraw_screen = false;

    draw_buffer();
    draw_statusbar();
    draw_minibuffer();
    draw_cursor();

    refresh();
}

void init_ui()
{
    // Initializion of ncurses is described here:
    // https://invisible-island.net/ncurses/man/ncurses.3x.html

    if (!initscr()) {
        error("Unable to init screen");
    }

    // Terminal settings, see:
    // https://invisible-island.net/ncurses/man/curs_inopts.3x.html

    raw();
    noecho();
    intrflush(stdscr, false);
    keypad(stdscr, true);

    // Enable color
    start_color();
    use_default_colors();
    assume_default_colors(-1, -1);
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // Finally draw the screen
    draw_screen();
}

void destroy_ui()
{
    endwin();
}
