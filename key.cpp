#include <ncurses.h>
#include "med.h"

extern bool redraw_screen;
extern bool edit_mode;
extern bool exit_app;

extern void debug(const std::string txt);
extern void reconcile_by_scrolling();
extern void insert_character(char c);

int parse_raw_key(int key)
{
    // Zero, Ctrl-Space?
    if (key == 0) {
        return 32 | ctrl_key;
    }

    // Tab/Return/Enter
    if (key == 9 || key == 10 || key == 13) {
        return key;
    } else if (key == 353) {
        return 9 | shift_key;
    }

    // Ctrl-a to Ctrl-z
    if (key >= 1 && key <= 26) {
        return (key + 96) | ctrl_key;
    }
    // Ctrl-h
    if (key == 263) {
        return 104 | ctrl_key;
    }
    // Ctrl-4 to Ctrl-7
    if (key >= 28 && key <= 31) {
        return (key + 24) | ctrl_key;
    }

    // Function keys
    if (key >= 277 && key <= 288) {
        return (key - 12) | shift_key;
    } else if (key >= 313 && key <= 324) {
        return (key - 48) | alt_key;
    } else if (key >= 325 && key <= 327) {
        return (key - 60) | shift_key | alt_key;
    } else if (key >= 289 && key <= 300) {
        return (key - 24) | ctrl_key;
    } else if (key >= 301 && key <= 312) {
        return (key - 36) | shift_key | ctrl_key;
    }

    switch (key) {
    // Arrow keys
    case 336: return KEY_DOWN | shift_key;
    case 524: return KEY_DOWN | alt_key;
    case 525: return KEY_DOWN | shift_key | alt_key;
    case 526: return KEY_DOWN | ctrl_key;
    case 527: return KEY_DOWN | shift_key | ctrl_key;
    case 528: return KEY_DOWN | alt_key | ctrl_key;

    case 337: return KEY_UP | shift_key;
    case 565: return KEY_UP | alt_key;
    case 566: return KEY_UP | shift_key | alt_key;
    case 567: return KEY_UP | ctrl_key;
    case 568: return KEY_UP | shift_key | ctrl_key;
    case 569: return KEY_UP | alt_key | ctrl_key;

    case 393: return KEY_LEFT | shift_key;
    case 544: return KEY_LEFT | alt_key;
    case 545: return KEY_LEFT | shift_key | alt_key;
    case 546: return KEY_LEFT | ctrl_key;
    case 547: return KEY_LEFT | shift_key | ctrl_key;
    case 548: return KEY_LEFT | alt_key | ctrl_key;

    case 402: return KEY_RIGHT | shift_key;
    case 559: return KEY_RIGHT | alt_key;
    case 560: return KEY_RIGHT | shift_key | alt_key;
    case 561: return KEY_RIGHT | ctrl_key;
    case 562: return KEY_RIGHT | shift_key | ctrl_key;
    case 563: return KEY_RIGHT | alt_key | ctrl_key;

    // Home etc.
    case 391: return KEY_HOME | shift_key;
    case 534: return KEY_HOME | alt_key;
    case 535: return KEY_HOME | shift_key | alt_key;
    case 536: return KEY_HOME | ctrl_key;
    case 537: return KEY_HOME | shift_key | ctrl_key;
    case 538: return KEY_HOME | alt_key | ctrl_key;

    case 383: return KEY_DC | shift_key;
    case 518: return KEY_DC | alt_key;
    case 519: return KEY_DC | shift_key | alt_key;
    case 520: return KEY_DC | ctrl_key;
    case 521: return KEY_DC | shift_key | ctrl_key;
    case 522: return KEY_DC | alt_key | ctrl_key;

    case 396: return KEY_NPAGE | shift_key;
    case 549: return KEY_NPAGE | alt_key;
    case 550: return KEY_NPAGE | shift_key | alt_key;
    case 551: return KEY_NPAGE | ctrl_key;
    case 552: return KEY_NPAGE | shift_key | ctrl_key;
    case 553: return KEY_NPAGE | alt_key | ctrl_key;

    case 398: return KEY_PPAGE | shift_key;
    case 554: return KEY_PPAGE | alt_key;
    case 555: return KEY_PPAGE | shift_key | alt_key;
    case 556: return KEY_PPAGE | ctrl_key;
    case 557: return KEY_PPAGE | shift_key | ctrl_key;
    case 558: return KEY_PPAGE | alt_key | ctrl_key;

    case 386: return KEY_END | shift_key;
    case 529: return KEY_END | alt_key;
    case 530: return KEY_END | shift_key | alt_key;
    case 531: return KEY_END | ctrl_key;
    case 532: return KEY_END | shift_key | ctrl_key;
    case 533: return KEY_END | alt_key | ctrl_key;

    // Everything else is returned as-is
    default: return key;
    }
}

int read_one_key()
{
    return getch();
}

int read_key_no_delay()
{
    nodelay(stdscr, true);
    int key = read_one_key();
    nodelay(stdscr, false);
    return key;
}

int read_full_key()
{
    int k1 = read_one_key();

    if (k1 == 27) {
        // 27 is either ESC or ALT but we have
        // to read second key to find out.
        int k2 = read_key_no_delay();

        if (k2 != ERR) {
            return parse_raw_key(k2) | alt_key;
        }
    }

    return parse_raw_key(k1);
}

void process_input()
{
    int full_key = read_full_key();
    int key = base_key(full_key);

    // Resize window
    if (key == KEY_RESIZE) {
        redraw_screen = true;
        reconcile_by_scrolling();
        return;
    }

    if (edit_mode) {
        if (key == ';') {
            if (key == full_key) {
                // No modifiers, exit edit mode
                edit_mode = false;
            } else {
                insert_character(';');
            }
        } else if (key == full_key && key >= 32 && key <= 126) {
            // No modifiers, printable characters
            insert_character(key);
        }
    } else {
        // Command mode

        // We don't use modifiers - yet
        if (key != full_key) {
            return;
        }

        // Actions in command mode
        if (key == 'f') {
            edit_mode = true;
        } else if (key == 'q') {
            exit_app = true;
        }
    }
}
