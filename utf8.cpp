#include "med.h"

// Length of one UTF-8 character in bytes
constexpr int utf8_char_length(std::string_view str, int index, int end)
{
    char first = str[index++];
    int len = 1;

    if (first & 0b1100'0000 && index < end) {
        index++;
        len++;

        if (first & 0b1110'0000 && index < end) {
            index++;
            len++;

            if (first & 0b1111'0000 && index < end) {
                index++;
                len++;
            }
        }
    }

    return len;
}

constexpr int utf8_char_length_reverse(std::string_view str, int index, int end)
{
    char c = str[index--];
    int len = 1;

    // First bit is set, second bit is not set
    while (c & 0b1000'0000 && !(c & 0b0100'0000) && index >= end && len < 4) {
        c = str[index--];
        len++;
    }

    return len;
}

// Number of UTF-8 characters in string
int utf8_length_chars(std::string_view str, int index, int end)
{
    int len = 0;

    while (index < end) {
        index += utf8_char_length(str, index, end);
        len++;
    }

    return len;
}

// Number of bytes in x UTF-8 characters
int utf8_length_bytes(std::string_view str, int index, int chars)
{
    int end = static_cast<int>(str.length());
    int result = 0;
    int c = 0;

    while (index < end && c < chars) {
        int len = utf8_char_length(str, index, end);
        index += len;
        result += len;
        c++;
    }

    return result;
}

// Number of bytes in x previous UTF-8 characters
int utf8_length_bytes_reverse(std::string_view str, int index, int chars)
{
    int result = 0;
    int c = 0;

    while (index > 0 && c < chars) {
        int len = utf8_char_length_reverse(str, index, 0);
        index -= len;
        result += len;
        c++;
    }

    return result;
}
