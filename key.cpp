#include "med.h"

#include <tuple>
#include <ncurses.h>

extern PromptType show_prompt;
extern std::string prompt;

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

    // ALT with arrow keys returns weird character codes
    // so translate them back to something normal here.
    if (key == 519) {
        key = KEY_DOWN;
        is_alt = true;
    } else if (key == 529) {
        key = KEY_UP;
        is_alt = true;
    } else if (key == 521) {
        key = KEY_LEFT;
        is_alt = true;
    } else if (key == 527) {
        key = KEY_RIGHT;
        is_alt = true;
    }

    return { key, is_alt };
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

    // Quit-prompt
    if (show_prompt == PromptType::quit) {
        if (key == 'q' || key == 'Q') {
            return InputResult::prompt_quit;
        } else if (key == 'y' || key == 'Y') {
            return InputResult::prompt_yes;
        } else if (key == 'n' || key == 'N') {
            return InputResult::prompt_no;
        }

        return InputResult::none;
    }

    // Goto prompt
    if (show_prompt == PromptType::goline) {
        // Quit with q
        if (key == 'q') {
            buffer.restore_point_location();
            show_prompt = PromptType::none;
        } else if (key == 10 || key == 13) {
            // Enter: perform goto line
            try {
                buffer.goto_line(std::stoi(prompt) - 1);
            } catch (...) {}
            show_prompt = PromptType::none;
        } else if (key == KEY_BACKSPACE) {
            if (prompt.length() > 0) {
                if (is_alt) {
                    // Alt-backspace erases all
                    prompt.clear();
                } else {
                    prompt.erase(prompt.length() - 1, 1);
                }
            }
        } else if (key >= '0' && key <= '9') {
            prompt.insert(prompt.length(), 1, key);
        }

        return InputResult::none;
    }

    // Search prompt
    if (show_prompt == PromptType::search) {
        // Quit with Alt-q
        if (key == 'q' && is_alt) {
            // Quit: restore point to location before search
            buffer.restore_point_location();
            show_prompt = PromptType::none;
        } else if (key == 10 || key == 13) {
            // Enter: keep point at current location
            show_prompt = PromptType::none;
        } else if ((key == 's' || key == 'n' || key == 'k') && is_alt) {
            // Search forward
            if (prompt.length() > 0) {
                buffer.search_forward(prompt);
            }
        } else if ((key == 'r' || key == 'p' || key == 'i') && is_alt) {
            // Search backward
            if (prompt.length() > 0) {
                buffer.search_backward(prompt);
            }
        } else if (key == KEY_BACKSPACE) {
            if (prompt.length() > 0) {
                if (is_alt) {
                    // Alt-backspace erases all
                    prompt.clear();
                } else {
                    prompt.erase(prompt.length() - 1, 1);
                }
            }
        } else if (key >= 32 && key <= 126) {
            // Printable characters
            prompt.insert(prompt.length(), 1, key);
        }

        return InputResult::none;
    }

    // Write prompt
    if (show_prompt == PromptType::write) {
        if (key == 'y' || key == 'Y') {
            buffer.write_file();
            show_prompt = PromptType::none;
        } else if (key == 'n' || key == 'N' || key == 'q') {
            show_prompt = PromptType::none;
        }

        return InputResult::none;
    }

    // -- not in prompt if we get here --

    // Enter command mode: alt-j
    if (buffer.get_edit_mode() && key == 'j' && is_alt) {
        buffer.set_edit_mode(false);
        return InputResult::none;
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
        } else if (key == 'g') {
            buffer.store_point_location();
            prompt.clear();
            show_prompt = PromptType::goline;
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
            show_prompt = PromptType::quit;
        } else if (key == 'r') {
            buffer.scroll_current_line_middle();
        } else if (key == 's') {
            buffer.store_point_location();
            prompt.clear();
            show_prompt = PromptType::search;
        } else if (key == 't') {
            buffer.delete_rest_of_line();
        } else if (key == 'v') {
            if (is_alt) {
                buffer.scroll_page_up();
            } else {
                buffer.scroll_page_down();
            }
        } else if (key == 'w') {
            show_prompt = PromptType::write;
        }

        return InputResult::none;
    }

    // Special keys
    if (key == KEY_UP) {
        if (is_alt) {
            buffer.backward_paragraph();
        } else {
            buffer.backward_line();
        }
    } else if (key == KEY_DOWN) {
        if (is_alt) {
            buffer.forward_paragraph();
        } else {
            buffer.forward_line();
        }
    } else if (key == KEY_LEFT) {
        if (is_alt) {
            buffer.backward_word();
        } else {
            buffer.backward_character();
        }
    } else if (key == KEY_RIGHT) {
        if (is_alt) {
            buffer.forward_word();
        } else {
            buffer.forward_character();
        }
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
        if (is_alt) {
            buffer.delete_word_backward();
        } else {
            buffer.delete_character_backward();
        }
    } else if (key == ',' && is_alt) {
        buffer.scroll_left();
    } else if (key == '.' && is_alt) {
        buffer.scroll_right();
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
