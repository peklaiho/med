#include <string>
#include <ncurses.h>

extern bool redraw_screen;
extern bool edit_mode;
extern bool exit_app;

extern void reconcile_by_scrolling();
extern void forward_character();
extern void backward_character();
extern void forward_word();
extern void backward_word();
extern void forward_paragraph();
extern void backward_paragraph();
extern void begin_of_line();
extern void end_of_line();
extern void begin_of_buffer();
extern void end_of_buffer();
extern void forward_line();
extern void backward_line();
extern void back_to_indentation();
extern void scroll_up();
extern void scroll_down();
extern void scroll_left();
extern void scroll_right();
extern void scroll_current_line_middle();
extern void scroll_page_up();
extern void scroll_page_down();
extern void insert_character(char c);
extern void delete_character_forward();
extern void delete_character_backward();
extern void delete_word_forward();
extern void delete_word_backward();
extern void delete_rest_of_line();

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
        reconcile_by_scrolling();
        return;
    }

    // Command mode: keys a to z are reserved
    // for special commands.
    if (!edit_mode && key >= 'a' && key <= 'z') {
        if (key == 'a') {
            if (is_alt) {
                begin_of_buffer();
            } else {
                begin_of_line();
            }
        } if (key == 'b') {
            back_to_indentation();
        } else if (key == 'd') {
            if (is_alt) {
                delete_word_forward();
            } else {
                delete_character_forward();
            }
        } else if (key == 'e') {
            if (is_alt) {
                end_of_buffer();
            } else {
                end_of_line();
            }
        } else if (key == 'f') {
            edit_mode = true;
        } else if (key == 'h') {
            if (is_alt) {
                delete_word_backward();
            } else {
                delete_character_backward();
            }
        } else if (key == 'i') {
            if (is_alt) {
                backward_paragraph();
            } else {
                backward_line();
            }
        } else if (key == 'j') {
            if (is_alt) {
                backward_word();
            } else {
                backward_character();
            }
        } else if (key == 'k') {
            if (is_alt) {
                forward_paragraph();
            } else {
                forward_line();
            }
        } else if (key == 'l') {
            if (is_alt) {
                forward_word();
            } else {
                forward_character();
            }
        } else if (key == 'q') {
            exit_app = true;
        } else if (key == 'r') {
            scroll_current_line_middle();
        } else if (key == 't') {
            delete_rest_of_line();
        } else if (key == 'v') {
            if (is_alt) {
                scroll_page_up();
            } else {
                scroll_page_down();
            }
        }

        return;
    }

    // Special keys
    if (key == KEY_UP) {
        backward_line();
    } else if (key == KEY_DOWN) {
        forward_line();
    } else if (key == KEY_LEFT) {
        backward_character();
    } else if (key == KEY_RIGHT) {
        forward_character();
    } else if (key == KEY_PPAGE) {
        scroll_page_up();
    } else if (key == KEY_NPAGE) {
        scroll_page_down();
    } else if (key == KEY_HOME) {
        begin_of_line();
    } else if (key == KEY_END) {
        end_of_line();
    } else if (key == KEY_DC) {
        delete_character_forward();
    } else if (key == KEY_BACKSPACE) {
        delete_character_backward();
    } else if (key == ';') {
        if (is_alt) {
            insert_character(';');
        } else {
            edit_mode = false;
        }
    } else if (key == 10 || key == 13) {
        insert_character('\n');
    } else if (key == '\t') {
        insert_character('\t');
    } else if (key >= 32 && key <= 126) {
        // Printable characters
        insert_character(key);
    }
}
