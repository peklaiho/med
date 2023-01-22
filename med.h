// Common libraries
#include <string>
#include <vector>

// Data types for line indices

using line_index_pair = unsigned long long;
using line_index = unsigned int;

static_assert(sizeof(line_index) == 4);
static_assert(sizeof(line_index_pair) == 8);

constexpr line_index line_start(line_index_pair pair)
{
    return static_cast<line_index>(pair >> 32);
}

constexpr line_index line_end(line_index_pair pair)
{
    return static_cast<line_index>(pair);
}

constexpr line_index_pair make_line_index(line_index start, line_index end)
{
    return (static_cast<line_index_pair>(start) << 32) |
        static_cast<line_index_pair>(end);
}

static_assert(line_start(make_line_index(123, 456)) == 123);
static_assert(line_end(make_line_index(123, 456)) == 456);

// Keys

constexpr int ctrl_key = 1 << 28;
constexpr int alt_key = 1 << 29;
constexpr int shift_key = 1 << 30;

constexpr bool is_ctrl(int key)
{
    return (key & ctrl_key) != 0;
}

constexpr bool is_alt(int key)
{
    return (key & alt_key) != 0;
}

constexpr bool is_shift(int key)
{
    return (key & shift_key) != 0;
}

constexpr int base_key(int key)
{
    return key &
        (~ctrl_key) &
        (~alt_key) &
        (~shift_key);
}

// Other data types

using cursor_position = unsigned int;

constexpr char newline = '\n';
