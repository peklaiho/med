#include "med.h"

// Length of one UTF-8 character in bytes
int utf8_char_length(std::string_view str, int index, int end)
{
    char first = str[index++];
    int len = 1;

    if (first & 0b1000'0000) {
        if (first & 0b1100'0000 && index < end) {
            index++;
            len++;
        }
        if (first & 0b1110'0000 && index < end) {
            index++;
            len++;
        }
        if (first & 0b1111'0000 && index < end) {
            index++;
            len++;
        }
    }

    return len;
}

// Get one UTF-8 character from string
// Use bit shift to store 4 bytes in one 32-bit (or larger) integer
int utf8_char(std::string_view str, int index, int end)
{
    char first = str[index++];
    int result = first;

    if (first & 0b1000'0000) {
        if (first & 0b1100'0000 && index < end) {
            result |= (str[index++] << 8);
        }
        if (first & 0b1110'0000 && index < end) {
            result |= (str[index++] << 16);
        }
        if (first & 0b1111'0000 && index < end) {
            result |= (str[index++] << 24);
        }
    }

    return result;
}

// Number of UTF-8 characters in string
int utf8_length(std::string_view str, int index, int end)
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
