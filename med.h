#include <string>
#include <vector>
#include <iostream>

enum class InputResult { none, next_buffer, prev_buffer, prompt_yes, prompt_no, prompt_quit, screen_size };
enum class PromptType { none, goline, search, quit };

class Buffer
{
private:
    std::string filename;
    std::string content;

    int screen_width = 0;
    int screen_height = 0;

    std::vector<int> line_indices;

    int point = 0;
    int previous_point = 0;
    int offset_line = 0;
    int offset_col = 0;
    int goal_col = 0;

    bool edit_mode = false;
    bool content_changed = false;

    void update_line_indices();

    // Setters
    void set_point(int value, bool reconcile, bool set_goal);
    bool set_line(int line, bool reconcile);
    void set_offset_line(int value, bool reconcile);
    void set_offset_col(int value, bool reconcile);

    // Reconcialition
    void reconcile_by_moving_point();
    void reconcile_by_scrolling();

    // Search helpers
    [[nodiscard]] int word_boundary_forward(int index) const;
    [[nodiscard]] int word_boundary_backward(int index) const;
    [[nodiscard]] int paragraph_boundary_forward(int index) const;
    [[nodiscard]] int paragraph_boundary_backward(int index) const;

public:
    // Constructor
    Buffer(std::string fname);

    // I/O
    void read_file();
    void write_file();

    // Getters
    [[nodiscard]] std::string get_filename() const;
    [[nodiscard]] std::string_view get_content() const;
    [[nodiscard]] int get_point() const;
    [[nodiscard]] int num_of_lines() const;
    [[nodiscard]] int line_start(int line) const;
    [[nodiscard]] int line_end(int line) const;
    [[nodiscard]] int current_line() const;
    [[nodiscard]] int current_col() const;
    [[nodiscard]] int get_offset_line() const;
    [[nodiscard]] int get_offset_col() const;
    [[nodiscard]] bool get_edit_mode() const;
    [[nodiscard]] bool get_content_changed() const;

    // Setters
    void set_screen_size(int width, int height);
    void set_edit_mode(bool value);
    void store_point_location();
    void restore_point_location();

    // Movement
    void begin_of_buffer();
    void end_of_buffer();
    void forward_character();
    void backward_character();
    void forward_word();
    void backward_word();
    void forward_paragraph();
    void backward_paragraph();
    void begin_of_line();
    void end_of_line();
    void forward_line();
    void backward_line();
    void back_to_indentation();
    void goto_line(int line);

    // Scrolling
    void scroll_up();
    void scroll_down();
    void scroll_left();
    void scroll_right();
    void scroll_current_line_middle();
    void scroll_page_up();
    void scroll_page_down();

    // Edit: insertion
    void insert_character(char c);

    // Edit: deletion
    void delete_character_forward();
    void delete_character_backward();
    void delete_word_forward();
    void delete_word_backward();
    void delete_rest_of_line();

    // Searching
    bool search_forward(std::string_view txt);
    bool search_backward(std::string_view txt);
};

class Screen
{
private:
    bool redraw_screen = false;

    void draw_buffer(const Buffer& buffer);
    void draw_statusbar(const Buffer& buffer);
    void draw_minibuffer();
    void draw_cursor(const Buffer& buffer);

public:
    Screen();
    ~Screen();

    void draw(Buffer& buffer);
    void size_changed();
};

class Keyboard
{
private:

public:
    InputResult read_input(Buffer& buffer);
};
