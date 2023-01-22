#include <ncurses.h>
#include "med.h"

extern std::string med_version;
extern std::string filename;
extern std::string content;
extern std::vector<line_index_pair> line_indices;
extern bool redraw_screen;
extern bool edit_mode;

extern void error(const std::string txt);

inline std::size_t line_count()
{
    return static_cast<std::size_t>(LINES);
}

inline std::size_t column_count()
{
    return static_cast<std::size_t>(COLS);
}

void draw_buffer()
{
    line_index start, end;
    std::size_t len;

    color_set(0, 0);

    for (std::size_t i = 0; i < line_indices.size() && i < (line_count() - 2); i++) {
        start = line_start(line_indices[i]);
        end = line_end(line_indices[i]);

        len = end - start;
        if (len > column_count()) {
            len = column_count();
        }

        mvaddnstr(i, 0, content.data() + start, len);
    }
}

void draw_statusbar()
{
    color_set(1, 0);

    std::string str(column_count(), ' ');

    if (edit_mode) {
        str.replace(2, 4, "EDIT");
    }

    str.replace(8, filename.length(), filename);

    str.replace(column_count() - 9, 3, "med");
    str.replace(column_count() - 5, 3, med_version);

    mvaddnstr(line_count() - 2, 0, str.data(), str.length());
}

void draw_minibuffer()
{
    color_set(0, 0);
}

void draw_cursor()
{
    move(0, 0);
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
    meta(stdscr, true);
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
