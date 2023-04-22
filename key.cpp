#include "med.h"

#include <ncurses.h>

extern bool exit_app;

int read_key_no_delay()
{
    nodelay(stdscr, true);
    int key = getch();
    nodelay(stdscr, false);
    return key;
}

void Keyboard::process_input(Screen& screen, Buffer& buffer)
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

    // Resize window
    if (key == KEY_RESIZE) {
        screen.size_changed();
        return;
    }

    // Handle prompt when exiting
    if (screen.get_show_prompt()) {
        if (key == 'q' || key == 'Q') {
            screen.set_show_prompt(false);
        } else if (key == 'y' || key == 'Y') {
            buffer.write_file();
            exit_app = true;
        } else if (key == 'n' || key == 'N') {
            exit_app = true;
        }
        return;
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
        } if (key == 'b') {
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
        } else if (key == 'q') {
            exit_app = true;
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

        return;
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
}
