# UTF-8 Support

Initially the editor supported only ASCII characters, but I wanted to add support for UTF-8. That was partly because I wanted to learn how UTF-8 encoding works.

I did not want to use a heavyweight external library like [ICU](https://unicode-org.github.io/icu/) or [Boost.Locale](https://www.boost.org/doc/libs/release/libs/locale/) because they feel like overkill for a simple app like this.

C++ has facilities for support of wide characters such as `wchar_t` and `std::wstring`. But apparently the [recommended way](https://utf8everywhere.org/) is not to use them for UTF-8, but to use regular `char` and `std::string` instead.

I also prefer to keep the buffer contents primarily as UTF-8 in memory and only decode the UTF-8 encoding as needed, for example when printing characters to screen or calculating column position. This saves memory compared to always keeping the entire buffer contents in memory as 32-bit characters.

Ncurses is another thing to [think about](http://dillingers.com/blog/2014/08/10/ncursesw-and-unicode/). There is a wide-char version of the library called `ncursesw`. But to what extent should I use that, if I don't use wide characters otherwise? Not so easy question to answer.
