#include "med.h"

#include <ncurses.h>

extern void error(std::string_view txt);
extern int utf8_length_bytes(std::string_view str, int index, int chars);
extern PromptType show_prompt;

constexpr std::string_view prompt_quit = "Save changes (y/n/q)? ";
constexpr std::string_view prompt_search = "Search: ";
constexpr std::string_view prompt_goline = "Goto line: ";
constexpr std::string_view prompt_write = "Write file (y/n)? ";

// Buffer
std::string prompt;
std::string buf;

[[nodiscard]] int get_screen_height()
{
    return LINES;
}

[[nodiscard]] int get_screen_width()
{
    return COLS;
}

constexpr int char_to_buf(std::string_view str, int index, int end)
{
    char first = str[index++];
    buf.append(1, first);
    int len = 1;

    if (first & 0b1000'0000) {
        if (first & 0b1100'0000 && index < end) {
            buf.append(1, str[index++]);
            len++;
        }
        if (first & 0b1110'0000 && index < end) {
            buf.append(1, str[index++]);
            len++;
        }
        if (first & 0b1111'0000 && index < end) {
            buf.append(1, str[index++]);
            len++;
        }
    }

    return len;
}

// Write given line to buf and return the length
void line_to_buf(const Buffer& buffer, const int line)
{
    int max_chars = get_screen_width();
    int chars = 0;

    buf.clear();

    auto content = buffer.get_content();
    int index = buffer.line_start(line);
    int end = buffer.line_end(line);

    // Skip over offset columns
    index += utf8_length_bytes(content, index, buffer.get_offset_col());

    while (chars < max_chars && index < end) {
        index += char_to_buf(content, index, end);
        chars++;
    }
}

void Screen::draw_buffer(const Buffer& buffer)
{
    color_set(0, 0);

    for (int row = 0; row < (get_screen_height() - 2); row++) {
        int line = row + buffer.get_offset_line();
        if (line >= buffer.num_of_lines()) {
            break;
        }

        line_to_buf(buffer, line);
        mvaddnstr(row, 0, buf.data(), buf.size());
    }
}

void Screen::draw_statusbar(const Buffer& buffer)
{
    color_set(1, 0);
    buf.clear();

    buf.append(buffer.get_content_changed() ? "  *" : "");
    buf.append(buffer.get_edit_mode() ? "  EDIT  " : "  ");
    buf.append(std::to_string(buffer.current_line() + 1));
    buf.append(":");
    buf.append(std::to_string(buffer.current_virtual_col()));
    buf.append("  ");
    buf.append(buffer.get_filename());

    // Fill remainder with spaces
    if (static_cast<int>(buf.size()) < get_screen_width()) {
        buf.append(get_screen_width() - buf.size(), ' ');
    }

    mvaddnstr(get_screen_height() - 2, 0, buf.data(), buf.size());
}

void Screen::draw_minibuffer()
{
    color_set(0, 0);

    if (show_prompt == PromptType::quit) {
        mvaddnstr(get_screen_height() - 1, 0, prompt_quit.data(), prompt_quit.size());
    } else if (show_prompt == PromptType::search) {
        mvaddnstr(get_screen_height() - 1, 0, prompt_search.data(), prompt_search.size());
        mvaddnstr(get_screen_height() - 1, prompt_search.size(), prompt.data(), prompt.size());
    } else if (show_prompt == PromptType::write) {
        mvaddnstr(get_screen_height() - 1, 0, prompt_write.data(), prompt_write.size());
    } else if (show_prompt == PromptType::goline) {
        mvaddnstr(get_screen_height() - 1, 0, prompt_goline.data(), prompt_goline.size());
        mvaddnstr(get_screen_height() - 1, prompt_goline.size(), prompt.data(), prompt.size());
    }
}

void Screen::draw_cursor(const Buffer& buffer)
{
    if (show_prompt == PromptType::quit) {
        move(get_screen_height() - 1, prompt_quit.size());
    } else if (show_prompt == PromptType::goline) {
        move(get_screen_height() - 1, prompt_goline.size() + prompt.size());
    } else if (show_prompt == PromptType::write) {
        move(get_screen_height() - 1, prompt_write.size());
    } else {
        move(buffer.current_line() - buffer.get_offset_line(),
             buffer.current_virtual_col() - buffer.get_offset_col());
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
