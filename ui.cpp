#include <string>
#include <ncurses.h>

extern void med_error(const std::string error);

void med_init_ui()
{
    if (!initscr()) {
        med_error("Unable to init screen");
    }

    cbreak();
    noecho();
    meta(stdscr, true);
    intrflush(stdscr, false);
    keypad(stdscr, true);
}

void med_destroy_ui()
{
    endwin();
}

int med_get_key()
{
    return getch();
}
