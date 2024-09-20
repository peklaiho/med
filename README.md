# med - text editor

*med* is a minimal [ncurses](https://invisible-island.net/ncurses/) based text editor written in C++.

## Philosophy

Before I created a simple text editor called [hacked](https://github.com/peklaiho/hacked) in Scheme (Lisp). This is essentially a more minimalistic version of the same but written in C++.

This editor contains minimal functionality currently and is only suitable for some quick-n-dirty editing of small files in the terminal. It could be used as a replacement for [nano](https://www.nano-editor.org/) or [vim](https://www.vim.org/) for example, but only if you are comfortable with the *ASCII* character set.

On the other hand, it is very lightweight and should run well on older systems with limited resources. The executable is currently only around 50 KiB when compiled on my machine.

## Compiling

Make sure you have a standard build environment installed to compile C++ programs (GCC, Make, and so on). The only external dependency is *ncurses* (the *ncurses.h* header file).

Simply type `make` to compile it. Then copy the resulting binary `med` into some directory that is in your *$PATH* (for example: *~/bin*).

## Usage

Type `med` followed by filenames to run it:

```
$ med readme.txt other.txt
```

## Keys

The default keybindings are set according to my own personal preferences. Any other user is encouraged to change the keybindings to their own preferences in `key.cpp` before compiling. A description of my default keys follows.

The editor has two modes, similar to *vim*: command mode and edit mode. Use <kbd>f</kbd> to enter edit mode and <kbd>Alt-j</kbd> to enter command mode.

Use <kbd>n</kbd> and <kbd>p</kbd> to switch to next and previous buffer respectively in command mode.

Use <kbd>i</kbd>, <kbd>j</kbd>, <kbd>k</kbd> and <kbd>l</kbd> to move around in command mode. When combined with <kbd>Alt</kbd>, they move one word or paragraph at a time. Use <kbd>v</kbd> to scroll down and <kbd>Alt-v</kbd> to scroll up. Use <kbd>r</kbd> to scroll the current line in the middle of the screen. Use <kbd>a</kbd> and <kbd>e</kbd> to move to the beginning and end of current line respectively. When combined with <kbd>Alt</kbd>, <kbd>a</kbd> and <kbd>e</kbd> move to the beginning and end of the whole file. Use <kbd>b</kbd> to move to the first character after indentation.

Use <kbd>d</kbd> and <kbd>h</kbd> to delete characters forward and backward respectively. Combine them with <kbd>Alt</kbd> to delete a whole word. Use <kbd>t</kbd> to delete the whole line starting from the cursor position.

Use <kbd>g</kbd> to go to a specific line. Use <kbd>q</kbd> to abort.

Use <kbd>s</kbd> to start search. Write the string to search for and press <kbd>Alt-s</kbd> to search forward or <kbd>Alt-r</kbd> to search backward. Hit return to exit search mode and keep cursor on current position. Use <kbd>Alt-q</kbd> to abort search and restore cursor to the position where search started. Note that searching is case-sensitive for now.

Use <kbd>w</kbd> to write the buffer contents into file. Use <kbd>q</kbd> to exit the editor. If any of the buffers have been modified, it will ask if you want to save changes.

## License

MIT
