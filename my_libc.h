#ifndef MY_STDLIB_H
#define MY_STDLIB_H

#include <stddef.h>   /* size_t, NULL (compiler-provided, no libc needed) */
#include <stdint.h>

/* ---------- ctype ---------- */
int my_isdigit(int c);
int my_isalpha(int c);
int my_isalnum(int c);
int my_isupper(int c);
int my_islower(int c);
int my_isspace(int c);
int my_isxdigit(int c);
int my_ispunct(int c);
int my_isprint(int c);
int my_toupper(int c);
int my_tolower(int c);

/* ---------- string / memory ---------- */
size_t my_strlen(const char *s);
size_t my_strnlen(const char *s, size_t n);
char  *my_strcpy(char *d, const char *s);
char  *my_strncpy(char *d, const char *s, size_t n);
char  *my_strcat(char *d, const char *s);
char  *my_strncat(char *d, const char *s, size_t n);
int    my_strcmp(const char *a, const char *b);
int    my_strncmp(const char *a, const char *b, size_t n);
char  *my_strchr(const char *s, int c);
char  *my_strrchr(const char *s, int c);
char  *my_strstr(const char *h, const char *n);
size_t my_strspn(const char *s, const char *accept);
size_t my_strcspn(const char *s, const char *reject);
char  *my_strpbrk(const char *s, const char *accept);
char  *my_strtok(char *s, const char *delim);
char  *my_strdup(const char *s);
size_t my_strlcpy(char *d, const char *s, size_t n);
void   my_strrev(char *s);

void  *my_memset(void *p, int c, size_t n);
void  *my_memcpy(void *d, const void *s, size_t n);
void  *my_memmove(void *d, const void *s, size_t n);
int    my_memcmp(const void *a, const void *b, size_t n);
void  *my_memchr(const void *p, int c, size_t n);

/* ---------- conversion ---------- */
int       my_atoi(const char *s);
long      my_atol(const char *s);
long      my_strtol(const char *s, char **end, int base);
unsigned long my_strtoul(const char *s, char **end, int base);
void      my_itoa(long value, char *buf, int base);

/* ---------- stdlib misc ---------- */
int    my_abs(int x);
long   my_labs(long x);
void   my_srand(unsigned seed);
int    my_rand(void);
void   my_qsort(void *base, size_t n, size_t size,
                int (*cmp)(const void *, const void *));
void  *my_bsearch(const void *key, const void *base, size_t n, size_t size,
                  int (*cmp)(const void *, const void *));

/* ---------- allocator (static heap, first-fit) ---------- */
void  *my_malloc(size_t n);
void   my_free(void *p);
void  *my_calloc(size_t n, size_t size);
void  *my_realloc(void *p, size_t n);

/* ---------- math ---------- */
double my_fabs(double x);
double my_pow_int(double x, int n);
double my_sqrt(double x);
double my_floor(double x);
double my_ceil(double x);
double my_fmod(double a, double b);

/* ---------- output (bring your own putchar) ---------- */
extern void (*my_putc_hook)(char c);   /* set this to e.g. a UART tx fn */
void my_puts(const char *s);
void my_putint(long v);

#endif
