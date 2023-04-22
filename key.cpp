#include "med.h"

#include <tuple>
#include <ncurses.h>

int read_key_no_delay()
{
    nodelay(stdscr, true);
    int key = getch();
    nodelay(stdscr, false);
    return key;
}

std::tuple<int, bool> read_key()
{
    int key = getch();
    bool is_alt = false;

    if (key == 27) {
        // 27 is either ESC or ALT but we have
        // to read second key to find out.
        int k2 = read_key_no_delay();

        if (k2 != ERR) {
            key = k2;
            is_alt = true;
        }
    }

    return { key, is_alt };
}

InputResult Keyboard::read_prompt()
{
    int key;
    bool is_alt;

    std::tie(key, is_alt) = read_key();

    // Resize window
    if (key == KEY_RESIZE) {
        return InputResult::screen_size;
    }

    if (key == 'q' || key == 'Q') {
        return InputResult::prompt_quit;
    } else if (key == 'y' || key == 'Y') {
        return InputResult::prompt_yes;
    } else if (key == 'n' || key == 'N') {
        return InputResult::prompt_no;
    }

    return InputResult::none;
}

InputResult Keyboard::read_input(Buffer& buffer)
{
    int key;
    bool is_alt;

    std::tie(key, is_alt) = read_key();

    // Resize window
    if (key == KEY_RESIZE) {
        return InputResult::screen_size;
    }

    // Command mode: keys a to z are reserved
    // for special commands.
    if (!buffer.get_edit_mode() && key >= 'a' && key <= 'z') {
        if (key == 'a') {
            if (is_alt) {
                buffer.begin_of_buffer();
            } else {
                buffer.begin_of_line();
            }
        } else if (key == 'b') {
            buffer.back_to_indentation();
        } else if (key == 'd') {
            if (is_alt) {
                buffer.delete_word_forward();
            } else {
                buffer.delete_character_forward();
            }
        } else if (key == 'e') {
            if (is_alt) {
                buffer.end_of_buffer();
            } else {
                buffer.end_of_line();
            }
        } else if (key == 'f') {
            buffer.set_edit_mode(true);
        } else if (key == 'h') {
            if (is_alt) {
                buffer.delete_word_backward();
            } else {
                buffer.delete_character_backward();
            }
        } else if (key == 'i') {
            if (is_alt) {
                buffer.backward_paragraph();
            } else {
                buffer.backward_line();
            }
        } else if (key == 'j') {
            if (is_alt) {
                buffer.backward_word();
            } else {
                buffer.backward_character();
            }
        } else if (key == 'k') {
            if (is_alt) {
                buffer.forward_paragraph();
            } else {
                buffer.forward_line();
            }
        } else if (key == 'l') {
            if (is_alt) {
                buffer.forward_word();
            } else {
                buffer.forward_character();
            }
        } else if (key == 'n') {
            return InputResult::next_buffer;
        } else if (key == 'p') {
            return InputResult::prev_buffer;
        } else if (key == 'q') {
            return InputResult::exit_app;
        } else if (key == 'r') {
            buffer.scroll_current_line_middle();
        } else if (key == 't') {
            buffer.delete_rest_of_line();
        } else if (key == 'v') {
            if (is_alt) {
                buffer.scroll_page_up();
            } else {
                buffer.scroll_page_down();
            }
        }

        return InputResult::none;
    }

    // Special keys
    if (key == KEY_UP) {
        buffer.backward_line();
    } else if (key == KEY_DOWN) {
        buffer.forward_line();
    } else if (key == KEY_LEFT) {
        buffer.backward_character();
    } else if (key == KEY_RIGHT) {
        buffer.forward_character();
    } else if (key == KEY_PPAGE) {
        buffer.scroll_page_up();
    } else if (key == KEY_NPAGE) {
        buffer.scroll_page_down();
    } else if (key == KEY_HOME) {
        buffer.begin_of_line();
    } else if (key == KEY_END) {
        buffer.end_of_line();
    } else if (key == KEY_DC) {
        buffer.delete_character_forward();
    } else if (key == KEY_BACKSPACE) {
        buffer.delete_character_backward();
    } else if (key == ',' && is_alt) {
        buffer.scroll_left();
    } else if (key == '.' && is_alt) {
        buffer.scroll_right();
    } else if (key == ';') {
        if (is_alt) {
            buffer.insert_character(';');
        } else {
            buffer.set_edit_mode(false);
        }
    } else if (key == 10 || key == 13) {
        buffer.insert_character('\n');
    } else if (key == '\t') {
        buffer.insert_character('\t');
    } else if (key >= 32 && key <= 126) {
        // Printable characters
        buffer.insert_character(key);
    }

    return InputResult::none;
}
