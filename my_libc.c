#include "my_libc.h"

int my_isdigit(int c){
    return (c >= '0' && c <= '9');
}

int my_isalpha(int c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int my_isalnum(int c){
    return (my_isdigit(c) || my_isalpha(c));
}

int my_isupper(int c){
    return (c >= 'A' && c <= 'Z');
}

int my_islower(int c){
    return (c >= 'a' && c <= 'z');
}

int my_isspace(int c){
    return (c == ' ' || c == '\n' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int my_isxdigit(int c){
    return (my_isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

int my_ispunct(int c){
    return ((c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~'));
}

int my_isprint(int c){
    return ((c >= ' ' && c <= '~'));
}

int my_toupper(int c){
    if(!my_isalpha(c) || my_isupper(c)) return c;

    return (c - 32u);

}

int my_tolower(int c){
    if(!my_isalpha(c) || my_islower(c)) return c;

    return (c+32);
}
