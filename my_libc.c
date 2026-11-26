#include "my_libc.h"
#include <stddef.h>
#include <stdint.h>

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

size_t my_strspn(const char *s, const char *accept){
    char set[256]={0};
    size_t i = 0;
    for(;accept[i] != '\0'; ++i){
        set[(unsigned char)accept[i]] = 1;
    }

    i=0;

    while(*s){
        if(set[(unsigned char)*s]) ++i;
        else break;
        ++s;
    }
    return i;
}

size_t my_strcspn(const char *s, const char *reject){
    char set[256]={0};
    size_t i = 0;
    for(;reject[i] != '\0'; ++i){
        set[(unsigned char)reject[i]] = 1;
    }

    i=0;

    while(*s){
        if(!set[(unsigned char)*s]) ++i;
        else break;
        ++s;
    }
    return i;
}

char *my_strpbrk(const char *s, const char *accept){
    unsigned char set[256] = {0};

    for(; *accept; ++accept)
        set[(unsigned char)*accept] = 1;

    for(; *s; ++s) {
        if(set[(unsigned char)*s])
            return (char *)s;
    }

    return NULL;
}

char  *my_strtok(char *s, const char *delim){
    static char *x = NULL;
    if(s) x = s;
    if(!x) return NULL;

    x += my_strspn(x,delim);
    if(*x == '\0'){
        x = NULL;
        return NULL;
    }

    char *start = x;
    x += my_strcspn(x, delim);

    if(*x){
        *x = '\0';
        x++;
    }
    else{
        x = NULL;
    }
    return start;
}

char *my_strdup(const char *s);                     //Requires Malloc

size_t my_strlcpy(char *d, const char *s, size_t n){
    const char *start = s;

    if(n != 0){
        while(n>1 && *s){
            --n;
            *d++ = *s++;
        }
        *d = '\0';
    }

    while (*s) ++s;
    return (size_t)(s-start);
}

void my_strrev(char *s){
    char* e = s;

    while(*e) e++;

    if (e == s)
        return;

    for(e--;s<e;s++,e--){
        *s = *s ^ *e;
        *e = *s ^ *e;
        *s = *s ^ *e;
    }
}

int my_atoi(const char *s){
    uint8_t sign = 0;
    int num = 0;

    while(my_isspace(*s)) ++s;

    if(*s == '+')   ++s;
    else if(*s == '-'){
        sign = 1;
        ++s;
    }

    while(my_isdigit(*s)){
        num = num*10 + (int)(*s-'0');
        ++s;
    }

    return (sign)?(-num):(num);
}

long my_atol(const char *s){
    uint8_t sign = 0;
    long int num = 0;

    while(my_isspace(*s)) ++s;

    if(*s == '+')   ++s;
    else if(*s == '-'){
        sign = 1;
        ++s;
    }

    while(my_isdigit(*s)){
        num = num*10 + (int)(*s-'0');
        ++s;
    }

    return (sign)?(-num):(num);
}

long my_strtol(const char *s, char **end, int base);
unsigned long my_strtoul(const char *s, char **end, int base);
void      my_itoa(long value, char *buf, int base);


#define MAX_ALLOC 200
#define ALIGN 8
#define SIZE (1024*1024)

typedef struct {
    size_t address;
    size_t size;
    uint8_t in_use;
}malloc_info_t;

static unsigned char heap[SIZE];
static size_t freespace = SIZE;
static size_t allocated = 0;
static malloc_info_t table[MAX_ALLOC] = {0};

void *my_malloc(size_t n){

    size_t index = 0;
    size_t j = 0;
    size_t gap = 0;
    size_t flag = 0;
    size_t heap_index = 0;
    void * address = NULL;

    if(allocated >= MAX_ALLOC || n == 0 || n > SIZE - (ALIGN - 1)) return NULL;

    n = (n + (ALIGN-1)) & ~(size_t)(ALIGN - 1);

    if(n > freespace) return NULL;

    for(j = 0; j < MAX_ALLOC; ++j){

        if(!table[j].in_use){
            index = j;
            heap_index = (j==0) ? 0 : (table[j-1].address + table[j-1].size);
            break;
        }

        if(j == 0 && table[0].address >= n){
            index = 0;
            heap_index = 0;
            flag = 1; 
            break;
        }

        if(j + 1 < MAX_ALLOC && table[j + 1].in_use){
            gap = table[j + 1].address - (table[j].address + table[j].size);
            if(gap >= n){
                index = j + 1;
                heap_index = table[j].address + table[j].size;
                flag = 1;
                break;
            }
        }
    }

    if(SIZE - heap_index < n){
        return NULL;
    }

    if(flag == 1){
        for(j = allocated; j > index; --j){
            table[j] = table[j - 1];
        }
    }

    address = &heap[heap_index];
    table[index].address = heap_index;
    table[index].size = n;
    table[index].in_use = 1;

    allocated += 1;
    freespace -= n;

    return address;
}

void my_free(void *ptr){

    size_t index = 0;
    size_t j = 0;
    size_t heap_index = 0;
    uintptr_t p = (uintptr_t)ptr;
    uintptr_t base = (uintptr_t)heap;

    if(ptr == NULL || allocated == 0 || p < base || p >= base + SIZE){
        return;
    }

    heap_index = (size_t)(p - base);

    for(index = 0; index < MAX_ALLOC; ++index){
        if(!table[index].in_use){
            break;
        }

        if(table[index].address == heap_index){
            freespace += table[index].size;

            for(j = index; j < allocated - 1; ++j){
                table[j] = table[j + 1];
            }

            allocated -= 1;
            table[allocated].address = 0;
            table[allocated].size = 0;
            table[allocated].in_use = 0;
            return;
        }
    }
}

void *my_calloc(size_t n, size_t size);
void *my_realloc(void *p, size_t n);
