#include <cstring>
#include <string>
#include <vector>
#include <ncurses.h>

extern std::string filename;
extern std::string content;
extern int point;
extern int offset_line;
extern int offset_col;
extern bool edit_mode;
extern bool content_changed;

extern void error(const std::string txt);
extern int line_start(int index);
extern int line_end(int index);
extern int num_of_lines();
extern int current_line();
extern int current_col();

bool redraw_screen = false;
bool show_prompt = false;

// Buffers
constexpr int line_buf_size = 4096;
constexpr int status_buf_size = 512;
char line_buf[line_buf_size + 1];
char status_buf[status_buf_size + 1];

int screen_height()
{
    return LINES;
}

int screen_width()
{
    return COLS;
}

// Write given line to line_buf and return the length
int line_to_buf(int line)
{
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

    return buf_idx;
}

int virtual_column()
{
    int start = line_start(current_line());
    int v_col = 0;

    for (int i = start; i < point; i++) {
        if (content[i] == '\t') {
            // Special handling for tabs
            v_col += TABSIZE - (v_col % TABSIZE);
        } else {
            v_col++;
        }
    }

    return v_col;
}

void draw_buffer()
{
    color_set(0, 0);

    for (int row = 0; row < (screen_height() - 2); row++) {
        int line = row + offset_line;
        if (line >= num_of_lines()) {
            break;
        }

        int len = line_to_buf(line) - offset_col;

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

    int max_len = status_buf_size;
    if (screen_width() < max_len) {
        max_len = screen_width();
    }

    std::snprintf(status_buf, max_len, "%s%s%6d:%-5d%s",
                  content_changed ? "  *" : "",
                  edit_mode ? "  EDIT" : "",
                  current_line() + 1,
                  current_col(),
                  filename.c_str());

    // Fill remainder with spaces
    int len = std::strlen(status_buf);
    while (len < max_len) {
        status_buf[len++] = ' ';
    }
    status_buf[len] = '\0';

    mvaddnstr(screen_height() - 2, 0, status_buf, len);
}

void draw_minibuffer()
{
    color_set(0, 0);

    if (show_prompt) {
        mvaddnstr(screen_height() - 1, 0, "Save changes (y/n/q)?", 21);
    }
}

void draw_cursor()
{
    if (show_prompt) {
        move(screen_height() - 1, 22);
    } else {
        move(current_line() - offset_line, virtual_column() - offset_col);
    }
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

    // Use 4 spaces for tabs
    set_tabsize(4);

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
