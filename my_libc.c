#include "my_libc.h"
#include <stddef.h>

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

size_t my_strlen(const char *s){
    size_t i = 0;
    while(s[i]!='\0'){
        ++i;
    }
    return i;
}

size_t my_strnlen(const char *s, size_t n){
    size_t i = 0;
    while(i < n && s[i] != '\0'){
        ++i;
    }
    return i;
}

char *my_strcpy(char *d, const char *s){
    size_t i = 0;
    while(*s != '\0'){
        d[i++] = *s++;
    }
    d[i] = '\0';

    return d;
}

char *my_strncpy(char *d, const char *s, size_t n){
    size_t i = 0;
    while(i < n && *s != '\0'){
        d[i++] = *s++;
    }

    while(i < n){
        d[i] = '\0';
        ++i;
    }

    return d;
}

char *my_strcat(char *d, const char *s){
    char *x = d+my_strlen(d);
    my_strncpy(x,s,my_strlen(s)+1);

    return d;
}

char *my_strncat(char *d, const char *s, size_t n){
    char *x = d+my_strlen(d);
    size_t i = 0;
    while(i<n && s[i] != '\0'){
        x[i] = s[i];
        ++i;
    }

    x[i] = '\0';

    return d;
}

int my_strcmp(const char *a, const char *b){
    size_t i = 0;
    while(a[i] != '\0' && b[i] != '\0'){
        if(a[i] == b[i]){
            ++i;
            continue;
        }
        else {
            return (unsigned char)a[i]-(unsigned char)b[i];
        }
    }
    return (unsigned char)a[i] - (unsigned char)b[i];
}

int my_strncmp(const char *a, const char *b, size_t n){
    size_t i = 0;
    while(i<n && a[i] != '\0' && b[i] != '\0'){
        if(a[i] == b[i]){
            ++i;
            continue;
        }
        else {
            return (unsigned char)a[i]-(unsigned char)b[i];
        }
    }

    if(i == n) return 0;

    return (unsigned char)a[i]-(unsigned char)b[i];
}

char  *my_strchr(const char *s, int c){
    char *x = (char *)s;
    while(*x != '\0'){
        if(*x == (char)c) return x;
        ++x;
    }
    if(c == '\0') return x;
    return NULL;
}

char *my_strrchr(const char *s, int c){
    char *x = (char *)s;
    char *y = NULL;

    while(*x != '\0'){
        if(*x == (char)c) y = x;
        ++x;
    }
    if(c == '\0') return x;
    return y;
}

char *my_strstr(const char *h, const char *n){
    size_t j = 0;

    if(!*n) return (char *)h;

    for(; *h; h++) {
        for(; n[j] && h[j] == n[j]; j++);
        if(!n[j]) return (char *)h;
    }
    return NULL;
}

size_t my_strspn(const char *s, const char *accept);
size_t my_strcspn(const char *s, const char *reject);
char  *my_strpbrk(const char *s, const char *accept);
char  *my_strtok(char *s, const char *delim);
char  *my_strdup(const char *s);
size_t my_strlcpy(char *d, const char *s, size_t n);
void   my_strrev(char *s);

