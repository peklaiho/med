#include "med.h"

#include <ncurses.h>

extern bool exit_app;
extern bool redraw_screen;
extern bool show_prompt;
extern std::unique_ptr<Buffer> current_buffer;

extern int get_screen_height();
extern int get_screen_width();

int read_key_no_delay()
{
    nodelay(stdscr, true);
    int key = getch();
    nodelay(stdscr, false);
    return key;
}

void process_input()
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
        redraw_screen = true;
        current_buffer->set_screen_size(get_screen_width(), get_screen_height());
        return;
    }

    // Handle prompt when exiting
    if (show_prompt) {
        if (key == 'q' || key == 'Q') {
            show_prompt = false;
        } else if (key == 'y' || key == 'Y') {
            current_buffer->write_file();
            exit_app = true;
        } else if (key == 'n' || key == 'N') {
            exit_app = true;
        }
        return;
    }

    // Command mode: keys a to z are reserved
    // for special commands.
    if (!current_buffer->get_edit_mode() && key >= 'a' && key <= 'z') {
        if (key == 'a') {
            if (is_alt) {
                current_buffer->begin_of_buffer();
            } else {
                current_buffer->begin_of_line();
            }
        } if (key == 'b') {
            current_buffer->back_to_indentation();
        } else if (key == 'd') {
            if (is_alt) {
                current_buffer->delete_word_forward();
            } else {
                current_buffer->delete_character_forward();
            }
        } else if (key == 'e') {
            if (is_alt) {
                current_buffer->end_of_buffer();
            } else {
                current_buffer->end_of_line();
            }
        } else if (key == 'f') {
            current_buffer->set_edit_mode(true);
        } else if (key == 'h') {
            if (is_alt) {
                current_buffer->delete_word_backward();
            } else {
                current_buffer->delete_character_backward();
            }
        } else if (key == 'i') {
            if (is_alt) {
                current_buffer->backward_paragraph();
            } else {
                current_buffer->backward_line();
            }
        } else if (key == 'j') {
            if (is_alt) {
                current_buffer->backward_word();
            } else {
                current_buffer->backward_character();
            }
        } else if (key == 'k') {
            if (is_alt) {
                current_buffer->forward_paragraph();
            } else {
                current_buffer->forward_line();
            }
        } else if (key == 'l') {
            if (is_alt) {
                current_buffer->forward_word();
            } else {
                current_buffer->forward_character();
            }
        } else if (key == 'q') {
            exit_app = true;
        } else if (key == 'r') {
            current_buffer->scroll_current_line_middle();
        } else if (key == 't') {
            current_buffer->delete_rest_of_line();
        } else if (key == 'v') {
            if (is_alt) {
                current_buffer->scroll_page_up();
            } else {
                current_buffer->scroll_page_down();
            }
        }

        return;
    }

    // Special keys
    if (key == KEY_UP) {
        current_buffer->backward_line();
    } else if (key == KEY_DOWN) {
        current_buffer->forward_line();
    } else if (key == KEY_LEFT) {
        current_buffer->backward_character();
    } else if (key == KEY_RIGHT) {
        current_buffer->forward_character();
    } else if (key == KEY_PPAGE) {
        current_buffer->scroll_page_up();
    } else if (key == KEY_NPAGE) {
        current_buffer->scroll_page_down();
    } else if (key == KEY_HOME) {
        current_buffer->begin_of_line();
    } else if (key == KEY_END) {
        current_buffer->end_of_line();
    } else if (key == KEY_DC) {
        current_buffer->delete_character_forward();
    } else if (key == KEY_BACKSPACE) {
        current_buffer->delete_character_backward();
    } else if (key == ',' && is_alt) {
        current_buffer->scroll_left();
    } else if (key == '.' && is_alt) {
        current_buffer->scroll_right();
    } else if (key == ';') {
        if (is_alt) {
            current_buffer->insert_character(';');
        } else {
            current_buffer->set_edit_mode(false);
        }
    } else if (key == 10 || key == 13) {
        current_buffer->insert_character('\n');
    } else if (key == '\t') {
        current_buffer->insert_character('\t');
    } else if (key >= 32 && key <= 126) {
        // Printable characters
        current_buffer->insert_character(key);
    }
}
