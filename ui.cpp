#include "med.h"

#include <cstring>
#include <ncurses.h>

extern void error(std::string_view txt);

// Buffer
constexpr int buf_size = 2048;
char buf[buf_size + 1];

[[nodiscard]] int get_screen_height()
{
    return LINES;
}

[[nodiscard]] int get_screen_width()
{
    return COLS;
}

// Write given line to buf and return the length
int line_to_buf(const Buffer& buffer, const int line)
{
    int buf_idx = 0;

    int max_len = buf_size;
    if (get_screen_width() < max_len) {
        max_len = get_screen_width();
    }

    auto content = buffer.get_content();

    for (int real_idx = buffer.line_start(line), virt_col = 0;
         buf_idx < max_len && real_idx < buffer.line_end(line);) {
        char c = content[real_idx++];

        if (c == '\t') {
            // Special handling for tabs
            int spaces_to_add = TABSIZE - (virt_col % TABSIZE);
            for (int s = 0; s < spaces_to_add && buf_idx < max_len; s++) {
                if (virt_col >= buffer.get_offset_col()) {
                    buf[buf_idx++] = ' ';
                }
                virt_col++;
            }
        } else {
            // Normal characters
            if (virt_col >= buffer.get_offset_col()) {
                buf[buf_idx++] = c;
            }
            virt_col++;
        }
    }

    // Terminate with null
    buf[buf_idx] = '\0';

    return buf_idx;
}

int virtual_column(const Buffer& buffer)
{
    int start = buffer.line_start(buffer.current_line());
    int v_col = 0;

    auto content = buffer.get_content();

    for (int i = start; i < buffer.get_point(); i++) {
        if (content[i] == '\t') {
            // Special handling for tabs
            v_col += TABSIZE - (v_col % TABSIZE);
        } else {
            v_col++;
        }
    }

    return v_col;
}

void Screen::draw_buffer(const Buffer& buffer)
{
    color_set(0, 0);

    for (int row = 0; row < (get_screen_height() - 2); row++) {
        int line = row + buffer.get_offset_line();
        if (line >= buffer.num_of_lines()) {
            break;
        }

        int len = line_to_buf(buffer, line);

        if (len > 0) {
            mvaddnstr(row, 0, buf, len);
        }
    }
}

void Screen::draw_statusbar(const Buffer& buffer)
{
    color_set(1, 0);

    int max_len = buf_size;
    if (get_screen_width() < max_len) {
        max_len = get_screen_width();
    }

    std::snprintf(buf, max_len, "%s%s  %d:%-d  %s",
                  buffer.get_content_changed() ? "  *" : "",
                  buffer.get_edit_mode() ? "  EDIT" : "",
                  buffer.current_line() + 1,
                  buffer.current_col(),
                  buffer.get_filename().c_str());

    // Fill remainder with spaces
    int len = std::strlen(buf);
    while (len < max_len) {
        buf[len++] = ' ';
    }
    buf[len] = '\0';

    mvaddnstr(get_screen_height() - 2, 0, buf, len);
}

void Screen::draw_minibuffer()
{
    color_set(0, 0);

    if (show_prompt) {
        mvaddnstr(get_screen_height() - 1, 0, "Save changes (y/n/q)?", 21);
    }
}

void Screen::draw_cursor(const Buffer& buffer)
{
    if (show_prompt) {
        move(get_screen_height() - 1, 22);
    } else {
        move(buffer.current_line() - buffer.get_offset_line(),
             virtual_column(buffer) - buffer.get_offset_col());
    }
}

void Screen::draw(Buffer& buffer)
{
    if (redraw_screen) {
        clear();
        redraw_screen = false;
    } else {
        erase();
    }

    buffer.set_screen_size(get_screen_width(), get_screen_height());

    draw_buffer(buffer);
    draw_statusbar(buffer);
    draw_minibuffer();
    draw_cursor(buffer);

    refresh();
}

void Screen::size_changed()
{
    redraw_screen = true;
}

bool Screen::get_show_prompt() const
{
    return show_prompt;
}

void Screen::set_show_prompt(bool value)
{
    show_prompt = value;
}

// Constructor
Screen::Screen()
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
}

// Destructor
Screen::~Screen()
{
    endwin();
}
