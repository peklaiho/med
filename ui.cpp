#include <string>
#include <vector>
#include <ncurses.h>

extern std::string med_version;
extern std::string filename;
extern std::string content;
extern int offset_line;
extern int offset_col;
extern bool edit_mode;

extern void error(const std::string txt);
extern int line_start(int index);
extern int line_end(int index);
extern int num_of_lines();
extern int current_line();
extern int current_col();

bool redraw_screen = false;

constexpr int line_buf_size = 4096;
char line_buf[line_buf_size + 1];

int screen_height()
{
    return LINES;
}

int screen_width()
{
    return COLS;
}

void draw_buffer()
{
    color_set(0, 0);

    for (int row = 0; row < (screen_height() - 2); row++) {
        int line = row + offset_line;
        if (line >= num_of_lines()) {
            break;
        }

        int buf_idx = 0, cont_idx = line_start(line);

        while (buf_idx < line_buf_size && cont_idx < line_end(line)) {
            char c = content[cont_idx++];

            if (c == '\t') {
                // Special handling for tabs
                int spaces_to_add = TABSIZE - (buf_idx % TABSIZE);
                for (int s = 0; s < spaces_to_add && buf_idx < line_buf_size; s++) {
                    line_buf[buf_idx++] = ' ';
                }
            } else {
                // Normal characters
                line_buf[buf_idx++] = c;
            }
        }

        // Terminate with null
        line_buf[buf_idx] = '\0';

        int len = buf_idx - offset_col;

        if (len > screen_width()) {
            len = screen_width();
        }

        if (len > 0) {
            mvaddnstr(row, 0, line_buf + offset_col, len);
        }
    }
}

void draw_statusbar()
{
    color_set(1, 0);

    std::string str(screen_width(), ' ');
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
    temp = std::to_string(TABSIZE);
    str.replace(col, temp.length(), temp);
    col += temp.length();
    str.replace(col, 1, ":");
    col++;
    temp = std::to_string(offset_col);
    str.replace(col, temp.length(), temp);
    col += temp.length() + 2;

    // Filename
    str.replace(col, filename.length(), filename);
    col += filename.length();

    mvaddnstr(screen_height() - 2, 0, str.data(), screen_width());
}

void draw_minibuffer()
{
    color_set(0, 0);
}

void draw_cursor()
{
    move(current_line() - offset_line, current_col() - offset_col);
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
