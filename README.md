# med - text editor

*med* is minimal [ncurses](https://invisible-island.net/ncurses/) based text editor written in C++ by [Pekka Laiho](http://laihoconsulting.com/).

## Philosophy

Before I created a simple text editor called [hacked](https://bitbucket.org/maddy83/hacked/) in Scheme (Lisp). This is essentially a more minimalistic version of the same but written in C++.

This editor contains minimal functionality currently and is only suitable for some quick-n-dirty editing of small files in the terminal. It could be used as a replacement for [nano](https://www.nano-editor.org/) or [vim](https://www.vim.org/) for example, but only if you are comfortable with the *ASCII* character set.

On the other hand, it is very lightweight and should run well on older systems with limited resources. The executable is currently only around 45 KiB when compiled on my machine.

## Compiling

Make sure you have a standard build environment installed to compile C++ programs (GCC, Make, and so on). The only external dependency is *ncurses* (the *ncurses.h* header file).

Simply type `make` to compile it. Then copy the resulting binary `med` into some directory that is in your *$PATH* (for example: *~/bin*).

## Usage

Type `med` followed by a filename to run it:

```
$ med readme.txt
```

## Keys

The default keybindings are set according to my own personal preferences. Any other user is encouraged to change the keybindings to their own preferences in `key.cpp` before compiling. A description of my default keys follows.

The editor has two modes, similar to *vim*: command mode and edit mode. Use `f` to enter edit mode and `;` to enter command mode. `Alt-;` is used to input the literal `;` character.

Use `i`, `j`, `k` and `l` to move around in command mode. When combined with `alt`, they move one word or paragraph at a time. Use `v` to scroll down and `alt-v` to scroll up. Use `r` to scroll the current line in the middle of the screen. Use `a` and `e` to move to the beginning and end of current line respectively. When combined with `alt`, `a` and `e` move to the beginning and end of the whole file. Use `b` to move to the first character after indentation.

Use `d` and `h` to delete characters forward and backward respectively. Combine them with `alt` to delete a whole word. Use `t` to delete the whole line starting from the cursor position.

Use `q` to exit the editor. If the file has been modified, it will ask if you want to save changes.

## License

[MIT](https://bitbucket.org/maddy83/med/src/master/LICENSE)
