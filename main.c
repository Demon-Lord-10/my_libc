#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include "my_libc.h"

/* ===== Enable ONE function at a time (uncomment as you write it) ===== */

/* ctype */
#define T_ISDIGIT
#define T_ISALPHA  
#define T_ISALNUM
#define T_ISUPPER
#define T_ISLOWER
#define T_ISSPACE
#define T_ISXDIGIT
#define T_ISPUNCT
#define T_ISPRINT
#define T_TOUPPER
#define T_TOLOWER

/* memory */
/* #define T_MEMSET  */
/* #define T_MEMCPY  */
/* #define T_MEMMOVE */
/* #define T_MEMCMP  */
/* #define T_MEMCHR  */

/* string */
/* #define T_STRLEN  */
/* #define T_STRNLEN */
/* #define T_STRCPY  */
/* #define T_STRNCPY */
/* #define T_STRLCPY */
/* #define T_STRCAT  */   /* also tests strncat */
/* #define T_STRCMP  */   /* also tests strncmp */
/* #define T_STRCHR  */   /* also tests strrchr */
/* #define T_STRSTR  */
/* #define T_STRSPN  */   /* also tests strcspn */
/* #define T_STRPBRK */
/* #define T_STRTOK  */
/* #define T_STRDUP  */   /* needs malloc + free */
/* #define T_STRREV  */

/* conversion */
/* #define T_ATOI    */   /* also tests atol */
/* #define T_STRTOL  */
/* #define T_STRTOUL */
/* #define T_ITOA    */
/* #define T_ABS     */   /* also tests labs */

/* sort / search / rand */
/* #define T_QSORT   */
/* #define T_BSEARCH */
/* #define T_RAND    */

/* allocator */
/* #define T_MALLOC  */   /* also tests free */
/* #define T_CALLOC  */   /* needs malloc + free */
/* #define T_REALLOC */   /* needs malloc + free */

/* math (build with -lm) */
/* #define T_FABS  */
/* #define T_FLOOR */     /* also tests ceil */
/* #define T_SQRT  */
/* #define T_POW   */
/* #define T_FMOD  */

/* output (custom putc hook) */
/* #define T_OUTPUT */

/* ===================== test framework ===================== */

static int passed, failed;

#define CHECK(cond) do {                                                \
    if (cond) {                                                         \
        passed++;                                                       \
    } else {                                                            \
        failed++;                                                       \
        printf("FAIL line %d: %s\n", __LINE__, #cond);                  \
    }                                                                   \
} while (0)

#define SIGN(x) (((x) > 0) - ((x) < 0))

/* Exhaustive: -1 (EOF) through 255, compared against real libc */
#define CMP_BOOL(mine, real)                                            \
    for (int ch = -1; ch <= 255; ch++) {                                \
        int got = (mine(ch) != 0);                                      \
        int exp_val = (real(ch) != 0);                                  \
        if (got != exp_val) {                                           \
            failed++;                                                   \
            printf("FAIL " #mine "(%d / 0x%02x): got %d, expected %d\n",\
                   ch, ch & 0xff, got, exp_val);                        \
        } else {                                                        \
            passed++;                                                   \
        }                                                               \
    }

#define CMP_EXACT(mine, real)                                           \
    for (int ch = -1; ch <= 255; ch++) {                                \
        int got = mine(ch);                                             \
        int exp_val = real(ch);                                         \
        if (got != exp_val) {                                           \
            failed++;                                                   \
            printf("FAIL " #mine "(%d / 0x%02x): got %d, expected %d\n",\
                   ch, ch & 0xff, got, exp_val);                        \
        } else {                                                        \
            passed++;                                                   \
        }                                                               \
    }

/* ===================== ctype ===================== */

void test_ctype(void)
{
#ifdef T_ISDIGIT
    CMP_BOOL(my_isdigit, isdigit)
#endif
#ifdef T_ISALPHA
    CMP_BOOL(my_isalpha, isalpha)
#endif
#ifdef T_ISALNUM
    CMP_BOOL(my_isalnum, isalnum)
#endif
#ifdef T_ISUPPER
    CMP_BOOL(my_isupper, isupper)
#endif
#ifdef T_ISLOWER
    CMP_BOOL(my_islower, islower)
#endif
#ifdef T_ISSPACE
    CMP_BOOL(my_isspace, isspace)
#endif
#ifdef T_ISXDIGIT
    CMP_BOOL(my_isxdigit, isxdigit)
#endif
#ifdef T_ISPUNCT
    CMP_BOOL(my_ispunct, ispunct)
#endif
#ifdef T_ISPRINT
    CMP_BOOL(my_isprint, isprint)
#endif
#ifdef T_TOUPPER
    CMP_EXACT(my_toupper, toupper)
#endif
#ifdef T_TOLOWER
    CMP_EXACT(my_tolower, tolower)
#endif
}

/* ===================== memory ===================== */

void test_mem(void)
{
#ifdef T_MEMSET
    {
        unsigned char buf[32];
        for (int i = 0; i < 32; i++) buf[i] = 0x5A; /* canary fill */

        /* Normal fill + boundary canary check (no underflow/overflow) */
        CHECK(my_memset(buf + 4, 0xAA, 16) == buf + 4);
        CHECK(buf[3] == 0x5A && buf[4] == 0xAA && buf[19] == 0xAA && buf[20] == 0x5A);

        /* Value truncation: > 255 and negative int (-1 -> 0xFF) */
        my_memset(buf + 4, 0x137, 4);
        CHECK(buf[4] == 0x37 && buf[7] == 0x37 && buf[8] == 0xAA);
        my_memset(buf + 4, -1, 2);
        CHECK(buf[4] == 0xFF && buf[5] == 0xFF && buf[6] == 0x37);

        /* n = 0 must not modify anything */
        my_memset(buf + 4, 0x00, 0);
        CHECK(buf[4] == 0xFF);

        /* Single byte at unaligned address */
        my_memset(buf + 7, 0x11, 1);
        CHECK(buf[6] == 0x37 && buf[7] == 0x11 && buf[8] == 0xAA);
    }
#endif
#ifdef T_MEMCPY
    {
        /* Embedded NUL bytes + canary guard bytes around destination */
        const unsigned char src[8] = {'a', 0x00, 'b', 0xFF, 0x80, 'c', 0x00, 'd'};
        unsigned char dst[16];
        memset(dst, 0xCC, sizeof dst);

        CHECK(my_memcpy(dst + 3, src, 8) == dst + 3);
        CHECK(dst[2] == 0xCC && dst[11] == 0xCC); /* canaries intact */
        CHECK(memcmp(dst + 3, src, 8) == 0);

        /* n = 0 must leave destination untouched and return dst */
        CHECK(my_memcpy(dst + 3, "ZZZZZZZZ", 0) == dst + 3);
        CHECK(dst[3] == 'a');

        /* 1-byte copy */
        CHECK(my_memcpy(dst + 3, "X", 1) == dst + 3);
        CHECK(dst[3] == 'X' && dst[4] == 0x00);
    }
#endif
#ifdef T_MEMMOVE
    {
        /* 1. Forward overlap by 1 byte (classic trap if copying low-to-high) */
        char m1[16] = "!abcdefgh!", m2[16] = "!abcdefgh!";
        memmove(m1 + 2, m1 + 1, 7);
        CHECK(my_memmove(m2 + 2, m2 + 1, 7) == m2 + 2);
        CHECK(memcmp(m1, m2, 16) == 0);
        CHECK(m2[0] == '!' && m2[9] == '!'); /* canaries intact */

        /* 2. Backward overlap by 1 byte */
        char m3[16] = "!abcdefgh!", m4[16] = "!abcdefgh!";
        memmove(m3 + 1, m3 + 2, 7);
        CHECK(my_memmove(m4 + 1, m4 + 2, 7) == m4 + 1);
        CHECK(memcmp(m3, m4, 16) == 0);
        CHECK(m4[0] == '!' && m4[9] == '!');

        /* 3. Exact same pointer (d == s) */
        CHECK(my_memmove(m4 + 1, m4 + 1, 7) == m4 + 1);
        CHECK(memcmp(m3, m4, 16) == 0);

        /* 4. Overlapping with n = 0 */
        CHECK(my_memmove(m4 + 1, m4 + 3, 0) == m4 + 1);
        CHECK(memcmp(m3, m4, 16) == 0);

        /* 5. Disjoint buffers with embedded NULs */
        const char bin[6] = {'1', '\0', '2', '\0', '3', '4'};
        char out[6] = {0};
        my_memmove(out, bin, 6);
        CHECK(memcmp(out, bin, 6) == 0);
    }
#endif
#ifdef T_MEMCMP
    {
        CHECK(my_memcmp("abc", "abc", 3) == 0);
        CHECK(my_memcmp("abc", "abd", 3) < 0);
        CHECK(my_memcmp("abd", "abc", 3) > 0);
        CHECK(my_memcmp("Xbc", "Ybc", 3) < 0);       /* diff at index 0 */
        CHECK(my_memcmp("abX", "abY", 2) == 0);      /* diff just past n */
        CHECK(my_memcmp("abc", "xyz", 0) == 0);      /* n = 0 */
        CHECK(my_memcmp("\x80", "\x7f", 1) > 0);     /* unsigned char compare */
        CHECK(my_memcmp("\xff", "\x00", 1) > 0);     /* 0xFF > 0x00 */
        CHECK(my_memcmp("\x01", "\xff", 1) < 0);
        CHECK(my_memcmp("a\0b", "a\0c", 3) < 0);     /* must not stop at NUL */
        CHECK(my_memcmp("a\0c", "a\0b", 3) > 0);
        CHECK(my_memcmp("a\0b", "a\0b", 3) == 0);
    }
#endif
#ifdef T_MEMCHR
    {
        const char buf[] = {'a', 'b', '\0', 'c', (char)0x80, (char)0xFF, 'b'};
        CHECK(my_memchr(buf, 'a', 7) == buf + 0);    /* first byte */
        CHECK(my_memchr(buf, 'b', 7) == buf + 1);    /* returns FIRST match */
        CHECK(my_memchr(buf, '\0', 7) == buf + 2);   /* finds embedded NUL */
        CHECK(my_memchr(buf, 'c', 7) == buf + 3);    /* searches past NUL! */
        CHECK(my_memchr(buf, 0x80, 7) == buf + 4);   /* high-bit byte */
        CHECK(my_memchr(buf, -1, 7) == buf + 5);     /* negative int -> 0xFF */
        CHECK(my_memchr(buf, 'a' + 256, 7) == buf);  /* int truncated to uchar */
        CHECK(my_memchr(buf, 'c', 3) == NULL);       /* match at index n (out of bounds) */
        CHECK(my_memchr(buf, 'a', 0) == NULL);       /* n = 0 */
        CHECK(my_memchr(buf, 'z', 7) == NULL);       /* not found */
    }
#endif
}

/* ===================== string ===================== */

void test_string(void)
{
#ifdef T_STRLEN
    CHECK(my_strlen("") == 0);
    CHECK(my_strlen("a") == 1);
    CHECK(my_strlen("hello") == 5);
    CHECK(my_strlen("a\0b") == 1);
    CHECK(my_strlen("\x80\xff\x01") == 3);
#endif
#ifdef T_STRNLEN
    CHECK(my_strnlen("", 0) == 0);
    CHECK(my_strnlen("", 5) == 0);
    CHECK(my_strnlen("hello", 0) == 0);
    CHECK(my_strnlen("hello", 3) == 3);              /* stops at maxlen */
    CHECK(my_strnlen("hello", 5) == 5);              /* exact length */
    CHECK(my_strnlen("hello", 6) == 5);              /* maxlen == len + 1 */
    CHECK(my_strnlen("hello", 100) == 5);
    CHECK(my_strnlen("ab\0cd", 5) == 2);             /* stops at first NUL */
#endif
#ifdef T_STRCPY
    {
        char d[16];
        memset(d, 'X', sizeof d);
        CHECK(my_strcpy(d + 1, "hello") == d + 1);
        CHECK(d[0] == 'X');                          /* no underwrite */
        CHECK(strcmp(d + 1, "hello") == 0);
        CHECK(d[7] == 'X');                          /* no overwrite past NUL */

        CHECK(my_strcpy(d + 1, "") == d + 1);
        CHECK(d[1] == '\0' && d[2] == 'e');          /* stops right at NUL */

        CHECK(my_strcpy(d + 1, "a\0b") == d + 1);
        CHECK(d[1] == 'a' && d[2] == '\0' && d[3] == 'e');
    }
#endif
#ifdef T_STRNCPY
    {
        char p1[12], p2[12];

        /* 1. n > strlen(src): must pad with NULs up to n, leave rest alone */
        memset(p1, 'Z', sizeof p1);
        memset(p2, 'Z', sizeof p2);
        strncpy(p1 + 1, "ab", 6);
        CHECK(my_strncpy(p2 + 1, "ab", 6) == p2 + 1);
        CHECK(memcmp(p1, p2, sizeof p1) == 0);

        /* 2. n < strlen(src): NO NUL added, bytes past n untouched */
        memset(p1, 'Z', sizeof p1);
        memset(p2, 'Z', sizeof p2);
        strncpy(p1 + 1, "abcdef", 3);
        my_strncpy(p2 + 1, "abcdef", 3);
        CHECK(memcmp(p1, p2, sizeof p1) == 0);

        /* 3. n == strlen(src): NO NUL added */
        memset(p1, 'Z', sizeof p1);
        memset(p2, 'Z', sizeof p2);
        strncpy(p1 + 1, "abc", 3);
        my_strncpy(p2 + 1, "abc", 3);
        CHECK(memcmp(p1, p2, sizeof p1) == 0);

        /* 4. Empty src with n > 0: fills n NUL bytes */
        memset(p1, 'Z', sizeof p1);
        memset(p2, 'Z', sizeof p2);
        strncpy(p1 + 1, "", 4);
        my_strncpy(p2 + 1, "", 4);
        CHECK(memcmp(p1, p2, sizeof p1) == 0);

        /* 5. n = 0: writes nothing */
        memset(p2, 'Z', sizeof p2);
        my_strncpy(p2, "abc", 0);
        CHECK(p2[0] == 'Z');
    }
#endif
#ifdef T_STRLCPY
    {
        char buf[8];

        /* Truncation: n < strlen(s) + 1 */
        memset(buf, 'X', sizeof buf);
        CHECK(my_strlcpy(buf, "abcdef", 4) == 6);
        CHECK(memcmp(buf, "abc\0XXXX", 8) == 0);

        /* Exact fit: n == strlen(s) + 1 */
        memset(buf, 'X', sizeof buf);
        CHECK(my_strlcpy(buf, "abc", 4) == 3);
        CHECK(memcmp(buf, "abc\0XXXX", 8) == 0);

        /* Larger buffer: does NOT zero-pad past the NUL (unlike strncpy) */
        memset(buf, 'X', sizeof buf);
        CHECK(my_strlcpy(buf, "hi", sizeof buf) == 2);
        CHECK(memcmp(buf, "hi\0XXXXX", 8) == 0);

        /* n = 1: only writes NUL byte */
        memset(buf, 'X', sizeof buf);
        CHECK(my_strlcpy(buf, "hello", 1) == 5);
        CHECK(buf[0] == '\0' && buf[1] == 'X');

        /* n = 0: writes NOTHING (even if dst is NULL), still returns strlen(src) */
        memset(buf, 'X', sizeof buf);
        CHECK(my_strlcpy(buf, "hello", 0) == 5);
        CHECK(buf[0] == 'X');
        CHECK(my_strlcpy(NULL, "hello", 0) == 5);

        /* Empty src */
        CHECK(my_strlcpy(buf, "", sizeof buf) == 0);
        CHECK(buf[0] == '\0');
    }
#endif
#ifdef T_STRCAT
    {
        char c1[16];
        memset(c1, 'X', sizeof c1);
        strcpy(c1, "foo");
        CHECK(my_strcat(c1, "bar") == c1);
        CHECK(strcmp(c1, "foobar") == 0 && c1[7] == 'X');
        CHECK(my_strcat(c1, "") == c1);
        CHECK(strcmp(c1, "foobar") == 0);

        char c2[16] = "";
        CHECK(my_strcat(c2, "abc") == c2);
        CHECK(strcmp(c2, "abc") == 0);

        /* strncat: n < strlen(src) MUST still append a trailing NUL at d[len+n] */
        char c3[16];
        memset(c3, 'X', sizeof c3);
        strcpy(c3, "ab");
        CHECK(my_strncat(c3, "cdefg", 3) == c3);
        CHECK(memcmp(c3, "abcde\0XX", 8) == 0);

        /* strncat: n > strlen(src) stops at src NUL and does NOT zero-pad */
        memset(c3, 'X', sizeof c3);
        strcpy(c3, "ab");
        CHECK(my_strncat(c3, "c", 10) == c3);
        CHECK(memcmp(c3, "abc\0XXXX", 8) == 0);

        /* strncat: n = 0 */
        CHECK(my_strncat(c3, "zzz", 0) == c3);
        CHECK(strcmp(c3, "abc") == 0);
    }
#endif
#ifdef T_STRCMP
    {
        const char *pairs[][2] = {
            {"", ""}, {"a", ""}, {"", "a"}, {"abc", "abc"}, {"abc", "abd"},
            {"abd", "abc"}, {"ab", "abc"}, {"abc", "ab"},
            {"\x80", "\x7f"}, {"\xff", "\x01"}, {"\x01", "\xff"},
            {"a\0hidden1", "a\0hidden2"} /* must stop at NUL! */
        };
        for (size_t i = 0; i < sizeof pairs / sizeof pairs[0]; i++) {
            CHECK(SIGN(my_strcmp(pairs[i][0], pairs[i][1]))
                  == SIGN(strcmp(pairs[i][0], pairs[i][1])));
            for (size_t k = 0; k < 8; k++) {
                CHECK(SIGN(my_strncmp(pairs[i][0], pairs[i][1], k))
                      == SIGN(strncmp(pairs[i][0], pairs[i][1], k)));
            }
        }
        /* strncmp boundary: differs at index 3, tested with n = 3 vs n = 4 */
        CHECK(my_strncmp("abcX", "abcY", 3) == 0);
        CHECK(my_strncmp("abcX", "abcY", 4) < 0);
        CHECK(my_strncmp("abcY", "abcX", 4) > 0);
        CHECK(my_strncmp("abc", "xyz", 0) == 0);
    }
#endif
#ifdef T_STRCHR
    {
        const char *s = "abacad\x80z";
        CHECK(my_strchr(s, 'a') == s + 0);           /* first match */
        CHECK(my_strchr(s, 'd') == s + 5);
        CHECK(my_strchr(s, 'z') == s + 7);           /* last char */
        CHECK(my_strchr(s, '\0') == s + 8);          /* NUL terminator */
        CHECK(my_strchr(s, 0x80) == s + 6);          /* high-bit char as int */
        CHECK(my_strchr(s, (char)0x80) == s + 6);    /* high-bit as negative char */
        CHECK(my_strchr(s, 'a' + 256) == s + 0);     /* int cast to char */
        CHECK(my_strchr(s, 'q') == NULL);
        CHECK(my_strchr("", 'a') == NULL);
        CHECK(my_strchr("", '\0') != NULL);          /* empty string NUL */

        CHECK(my_strrchr(s, 'a') == s + 4);          /* LAST match */
        CHECK(my_strrchr(s, 'b') == s + 1);          /* single match */
        CHECK(my_strrchr(s, 'z') == s + 7);
        CHECK(my_strrchr(s, '\0') == s + 8);         /* NUL terminator */
        CHECK(my_strrchr(s, 0x80) == s + 6);
        CHECK(my_strrchr(s, 'q') == NULL);
        CHECK(my_strrchr("", 'a') == NULL);
        CHECK(my_strrchr("", '\0') != NULL);
    }
#endif
#ifdef T_STRSTR
    {
        const char *s = "hello world";
        CHECK(my_strstr(s, "hello") == s);           /* at start */
        CHECK(my_strstr(s, "world") == s + 6);       /* at end */
        CHECK(my_strstr(s, "d") == s + 10);          /* last char */
        CHECK(my_strstr(s, "") == s);                /* empty needle */
        CHECK(my_strstr(s, "xyz") == NULL);
        CHECK(my_strstr(s, "hello world!") == NULL); /* needle longer than haystack */
        CHECK(my_strstr(s, "world!") == NULL);       /* partial match at end */
        CHECK(my_strstr("", "a") == NULL);
        CHECK(my_strstr("", "") != NULL);

        /* Overlapping restart traps (breaks naive loops that don't backtrack) */
        const char *p1 = "aaab";
        CHECK(my_strstr(p1, "aab") == p1 + 1);
        const char *p2 = "abababc";
        CHECK(my_strstr(p2, "ababc") == p2 + 2);
        const char *p3 = "mississippi";
        CHECK(my_strstr(p3, "issip") == p3 + 4);
        const char *p4 = "aaaa";
        CHECK(my_strstr(p4, "aa") == p4);            /* returns FIRST occurrence */
    }
#endif
#ifdef T_STRSPN
    CHECK(my_strspn("aabbcc", "ab") == 4);
    CHECK(my_strspn("aabbcc", "ba") == 4);           /* order in accept doesn't matter */
    CHECK(my_strspn("aabbcc", "abc") == 6);          /* entire string matches */
    CHECK(my_strspn("aabbcc", "aaaa") == 2);         /* duplicate chars in accept */
    CHECK(my_strspn("xyz", "ab") == 0);
    CHECK(my_strspn("", "ab") == 0);
    CHECK(my_strspn("abc", "") == 0);
    CHECK(my_strspn("", "") == 0);

    CHECK(my_strcspn("hello", "l") == 2);
    CHECK(my_strcspn("hello", "ol") == 2);           /* stops at 'l' (idx 2), not 'o' (idx 4) */
    CHECK(my_strcspn("hello", "h") == 0);            /* first char rejected */
    CHECK(my_strcspn("hello", "z") == 5);
    CHECK(my_strcspn("hello", "") == 5);
    CHECK(my_strcspn("", "abc") == 0);
    CHECK(my_strcspn("", "") == 0);
#endif
#ifdef T_STRPBRK
    {
        const char *s = "hello world";
        /* Trap: 'o' is first in accept, but 'e' appears earlier in s! */
        CHECK(my_strpbrk(s, "oeh") == s + 0);
        CHECK(my_strpbrk(s, "oe") == s + 1);
        CHECK(my_strpbrk(s, "d") == s + 10);
        CHECK(my_strpbrk(s, "xyz") == NULL);
        CHECK(my_strpbrk(s, "") == NULL);
        CHECK(my_strpbrk("", "abc") == NULL);
        CHECK(my_strpbrk("", "") == NULL);
    }
#endif
#ifdef T_STRTOK
    {
        /* 1. Leading, consecutive, and trailing delimiters */
        char t1[] = "  ,;a,,b;c  ;";
        char *tok = my_strtok(t1, ",; ");
        CHECK(tok && strcmp(tok, "a") == 0);
        tok = my_strtok(NULL, ",; "); CHECK(tok && strcmp(tok, "b") == 0);
        tok = my_strtok(NULL, ",; "); CHECK(tok && strcmp(tok, "c") == 0);
        tok = my_strtok(NULL, ",; "); CHECK(tok == NULL);
        tok = my_strtok(NULL, ",; "); CHECK(tok == NULL); /* repeated NULL call safe */

        /* 2. Changing delimiter set between calls (required by C standard) */
        char t2[] = "one:two,three;four";
        tok = my_strtok(t2, ":");   CHECK(tok && strcmp(tok, "one") == 0);
        tok = my_strtok(NULL, ","); CHECK(tok && strcmp(tok, "two") == 0);
        tok = my_strtok(NULL, ";"); CHECK(tok && strcmp(tok, "three") == 0);
        tok = my_strtok(NULL, "."); CHECK(tok && strcmp(tok, "four") == 0);
        tok = my_strtok(NULL, "."); CHECK(tok == NULL);

        /* 3. Only delimiters, empty string, single token with no delimiters */
        char t3[] = ",,,";
        CHECK(my_strtok(t3, ",") == NULL);
        CHECK(my_strtok(NULL, ",") == NULL);
        char t4[] = "";
        CHECK(my_strtok(t4, ",") == NULL);
        char t5[] = "alone";
        tok = my_strtok(t5, ",");
        CHECK(tok && strcmp(tok, "alone") == 0);
        CHECK(my_strtok(NULL, ",") == NULL);
    }
#endif
#ifdef T_STRDUP
    {
        const char *orig = "copy me safely";
        char *dup = my_strdup(orig);
        CHECK(dup != NULL && dup != orig && strcmp(dup, orig) == 0);
        dup[0] = 'C';
        CHECK(orig[0] == 'c'); /* independent copy */
        my_free(dup);

        dup = my_strdup("");
        CHECK(dup != NULL && dup[0] == '\0');
        my_free(dup);
    }
#endif
#ifdef T_STRREV
    {
        char r0[] = "", r1[] = "a", r2[] = "ab", r3[] = "abc", r4[] = "abcd", r5[] = "racecar";
        my_strrev(r0); my_strrev(r1); my_strrev(r2); my_strrev(r3); my_strrev(r4); my_strrev(r5);
        CHECK(strcmp(r0, "") == 0);
        CHECK(strcmp(r1, "a") == 0);
        CHECK(strcmp(r2, "ba") == 0);
        CHECK(strcmp(r3, "cba") == 0);
        CHECK(strcmp(r4, "dcba") == 0);
        CHECK(strcmp(r5, "racecar") == 0);
    }
#endif
}

/* ===================== conversion ===================== */

void test_convert(void)
{
#ifdef T_ATOI
    {
        const char *inputs[] = {
            "0", "-0", "+0", "42", "-42", "+7",
            " \t\n\v\f\r12", " \t-99xyz", "12abc34", "abc", "", "   ",
            "-", "+", "++5", "--5", "+-5", "-+5", "- 5", "+ 5",
            "000007", "2147483647", "-2147483648", "99999 ", "3.14"
        };
        for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++) {
            if (my_atoi(inputs[i]) != atoi(inputs[i])) {
                failed++;
                printf("FAIL my_atoi(\"%s\"): got %d, expected %d\n",
                       inputs[i], my_atoi(inputs[i]), atoi(inputs[i]));
            } else {
                passed++;
            }
            if (my_atol(inputs[i]) != atol(inputs[i])) {
                failed++;
                printf("FAIL my_atol(\"%s\"): got %ld, expected %ld\n",
                       inputs[i], my_atol(inputs[i]), atol(inputs[i]));
            } else {
                passed++;
            }
        }
    }
#endif
#ifdef T_STRTOL
    {
        struct { const char *s; int base; } cases[] = {
            /* Standard bases & signs */
            {"0", 10}, {"-0", 10}, {"+0", 10}, {"123", 10}, {"-123xyz", 10},
            {" \t\n\v\f\r+9", 10}, {" \t-42 ", 10},
            /* Hex (base 16 and base 0) */
            {"ff", 16}, {"0xFF", 16}, {"0Xff", 16}, {"-0x10", 16}, {"+0x10", 16},
            {"0x1F", 0}, {"-0X1f", 0}, {"0x0", 0},
            /* Octal & decimal auto-detection (base 0) */
            {"0", 0}, {"-0", 0}, {"00", 0}, {"017", 0}, {"-017", 0},
            {"08", 0},    /* '8' invalid in octal -> returns 0, endptr at '8' */
            {"0789", 0},  /* stops at '8' -> returns 7, endptr at '8' */
            {"17", 0},
            /* Arbitrary bases (2..36) */
            {"101101", 2}, {"-101", 2}, {"12", 2}, /* stops at '2' */
            {"zz", 36}, {"-ZZ", 36}, {"a0", 11},
            /* Invalid / empty / prefix-only traps (endptr positioning!) */
            {"", 10}, {"   ", 10}, {"abc", 10}, {"   abc", 10},
            {"+", 10}, {"-", 10}, {"   +", 10}, {"   -", 10},
            {"+-5", 10}, {"++5", 10},
            {"0x", 16}, {"0x", 0}, {"   0x", 16}, {"-0x", 16}, {"0xG", 16},
            /* Limits */
            {"2147483647", 10}, {"-2147483648", 10}
        };
        for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
            char *e1 = NULL, *e2 = NULL;
            long ra = strtol(cases[i].s, &e1, cases[i].base);
            long rb = my_strtol(cases[i].s, &e2, cases[i].base);
            if (ra != rb || (e1 - cases[i].s) != (e2 - cases[i].s)) {
                failed++;
                printf("FAIL my_strtol(\"%s\", base %d): real=%ld (end+%ld) mine=%ld (end+%ld)\n",
                       cases[i].s, cases[i].base,
                       ra, (long)(e1 - cases[i].s),
                       rb, (long)(e2 - cases[i].s));
            } else {
                passed++;
            }
        }
        CHECK(my_strtol("42", NULL, 10) == 42); /* NULL endptr must not crash */
        CHECK(my_strtol("invalid", NULL, 10) == 0);
    }
#endif
#ifdef T_STRTOUL
    {
        struct { const char *s; int base; } cases[] = {
            {"0", 10}, {"42", 10}, {"0xFF", 16}, {"0x10", 0}, {"017", 0},
            {"", 10}, {"   ", 10}, {"   +", 10}, {"0x", 16}, {"1011", 2}, {"zz", 36}
        };
        for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++) {
            char *e1 = NULL, *e2 = NULL;
            unsigned long ra = strtoul(cases[i].s, &e1, cases[i].base);
            unsigned long rb = my_strtoul(cases[i].s, &e2, cases[i].base);
            CHECK(ra == rb && (e1 - cases[i].s) == (e2 - cases[i].s));
        }
        CHECK(my_strtoul("42", NULL, 10) == 42UL);
    }
#endif
#ifdef T_ITOA
    {
        /* Base 10 including LONG_MIN (negation overflow trap!) and LONG_MAX */
        long vals[] = {
            0, 1, -1, 9, -9, 10, -10, 99, -99, 100, -100,
            255, 123456789L, -123456789L, LONG_MAX, LONG_MIN
        };
        char b1[80], b2[80];
        for (size_t i = 0; i < sizeof vals / sizeof vals[0]; i++) {
            snprintf(b1, sizeof b1, "%ld", vals[i]);
            memset(b2, 'X', sizeof b2);
            my_itoa(vals[i], b2, 10);
            if (strcmp(b1, b2) != 0) {
                failed++;
                printf("FAIL my_itoa(%ld, 10): got \"%s\", expected \"%s\"\n",
                       vals[i], b2, b1);
            } else {
                passed++;
            }
        }
        /* Non-decimal bases */
        my_itoa(0, b2, 2);     CHECK(strcmp(b2, "0") == 0);
        my_itoa(0, b2, 16);    CHECK(strcmp(b2, "0") == 0);
        my_itoa(0, b2, 36);    CHECK(strcmp(b2, "0") == 0);
        my_itoa(1, b2, 2);     CHECK(strcmp(b2, "1") == 0);
        my_itoa(255, b2, 2);   CHECK(strcmp(b2, "11111111") == 0);
        my_itoa(255, b2, 8);   CHECK(strcmp(b2, "377") == 0);
        my_itoa(255, b2, 16);  CHECK(strcmp(b2, "ff") == 0);
        my_itoa(4096, b2, 16); CHECK(strcmp(b2, "1000") == 0);
        my_itoa(35, b2, 36);   CHECK(strcmp(b2, "z") == 0);
        my_itoa(36, b2, 36);   CHECK(strcmp(b2, "10") == 0);
    }
#endif
#ifdef T_ABS
    CHECK(my_abs(0) == 0);
    CHECK(my_abs(1) == 1 && my_abs(-1) == 1);
    CHECK(my_abs(INT_MAX) == INT_MAX);
    CHECK(my_abs(INT_MIN + 1) == INT_MAX);

    CHECK(my_labs(0L) == 0L);
    CHECK(my_labs(1L) == 1L && my_labs(-1L) == 1L);
    CHECK(my_labs(LONG_MAX) == LONG_MAX);
    CHECK(my_labs(LONG_MIN + 1L) == LONG_MAX);
#endif
}

/* ===================== sort / search / rand ===================== */

#if defined(T_QSORT) || defined(T_BSEARCH)
int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

int cmp_char(const void *a, const void *b)
{
    unsigned char x = *(const unsigned char *)a;
    unsigned char y = *(const unsigned char *)b;
    return (x > y) - (x < y);
}
#endif

#ifdef T_QSORT
struct item_13 {
    int key;
    char pad[9]; /* 13-byte struct to catch non-power-of-two size bugs */
};

int cmp_item13(const void *a, const void *b)
{
    int x = ((const struct item_13 *)a)->key;
    int y = ((const struct item_13 *)b)->key;
    return (x > y) - (x < y);
}

void sort_case(int *arr, size_t n)
{
    int ref[64];
    if (n > 0) memcpy(ref, arr, n * sizeof *arr);
    qsort(ref, n, sizeof *ref, cmp_int);
    my_qsort(arr, n, sizeof *arr, cmp_int);
    CHECK(n == 0 || memcmp(ref, arr, n * sizeof *arr) == 0);
}
#endif

void test_sort(void)
{
#ifdef T_QSORT
    {
        int empty[1] = {42};
        my_qsort(empty, 0, sizeof *empty, cmp_int);
        CHECK(empty[0] == 42); /* n = 0 untouched */

        int one[]    = {5};                              sort_case(one, 1);
        int two_u[]  = {2, 1};                           sort_case(two_u, 2);
        int two_s[]  = {1, 2};                           sort_case(two_s, 2);
        int two_eq[] = {4, 4};                           sort_case(two_eq, 2);
        int sorted[] = {1, 2, 3, 4, 5, 6, 7};            sort_case(sorted, 7);
        int rev[]    = {7, 6, 5, 4, 3, 2, 1};            sort_case(rev, 7);
        int dups[]   = {3, 1, 3, 1, 3, 1, 2};            sort_case(dups, 7);
        int same[]   = {7, 7, 7, 7, 7, 7, 7, 7};         sort_case(same, 8);
        int pipe[]   = {1, 2, 3, 4, 4, 3, 2, 1};         sort_case(pipe, 8);
        int limits[] = {0, -5, 5, INT_MIN, INT_MAX, -1}; sort_case(limits, 6);

        /* Test 1-byte elements (catches hardcoded sizeof(int) swaps) */
        char c_arr[] = "zyxwvutsrqponmlkjihgfedcba";
        my_qsort(c_arr, 26, 1, cmp_char);
        CHECK(strcmp(c_arr, "abcdefghijklmnopqrstuvwxyz") == 0);

        /* Test odd-sized structs (verify payload bytes move with keys) */
        struct item_13 items[4] = {
            {30, "thirty"}, {10, "ten"}, {40, "forty"}, {20, "twenty"}
        };
        my_qsort(items, 4, sizeof items[0], cmp_item13);
        CHECK(items[0].key == 10 && strcmp(items[0].pad, "ten") == 0);
        CHECK(items[1].key == 20 && strcmp(items[1].pad, "twenty") == 0);
        CHECK(items[2].key == 30 && strcmp(items[2].pad, "thirty") == 0);
        CHECK(items[3].key == 40 && strcmp(items[3].pad, "forty") == 0);

        /* 200 random fuzz arrays */
        srand(1);
        for (int round = 0; round < 200; round++) {
            int arr[64];
            size_t n = (size_t)(rand() % 64) + 1;
            for (size_t i = 0; i < n; i++) arr[i] = rand() % 20 - 10;
            sort_case(arr, n);
        }
    }
#endif
#ifdef T_BSEARCH
    {
        /* Odd and even element counts */
        int odd[]  = {10, 20, 30, 40, 50};
        int even[] = {10, 20, 30, 40, 50, 60};

        for (int i = 0; i < 5; i++)
            CHECK(my_bsearch(&odd[i], odd, 5, sizeof odd[0], cmp_int) == &odd[i]);
        for (int i = 0; i < 6; i++)
            CHECK(my_bsearch(&even[i], even, 6, sizeof even[0], cmp_int) == &even[i]);

        /* Misses in every gap (below min, between elements, above max) */
        int misses[] = {INT_MIN, 5, 15, 25, 35, 45, 55, 65, INT_MAX};
        for (size_t i = 0; i < sizeof misses / sizeof misses[0]; i++) {
            CHECK(my_bsearch(&misses[i], odd, 5, sizeof odd[0], cmp_int) == NULL);
            CHECK(my_bsearch(&misses[i], even, 6, sizeof even[0], cmp_int) == NULL);
        }

        /* Boundary sizes: n = 0, n = 1, n = 2 */
        int k10 = 10, k20 = 20, k5 = 5, k15 = 15, k25 = 25;
        CHECK(my_bsearch(&k10, even, 0, sizeof even[0], cmp_int) == NULL);
        CHECK(my_bsearch(&k10, even, 1, sizeof even[0], cmp_int) == &even[0]);
        CHECK(my_bsearch(&k5,  even, 1, sizeof even[0], cmp_int) == NULL);
        CHECK(my_bsearch(&k20, even, 1, sizeof even[0], cmp_int) == NULL);

        CHECK(my_bsearch(&k10, even, 2, sizeof even[0], cmp_int) == &even[0]);
        CHECK(my_bsearch(&k20, even, 2, sizeof even[0], cmp_int) == &even[1]);
        CHECK(my_bsearch(&k5,  even, 2, sizeof even[0], cmp_int) == NULL);
        CHECK(my_bsearch(&k15, even, 2, sizeof even[0], cmp_int) == NULL);
        CHECK(my_bsearch(&k25, even, 2, sizeof even[0], cmp_int) == NULL);

        /* Non-int element size (1-byte chars) */
        const char chars[] = "acegi";
        char target = 'g', miss_c = 'b';
        CHECK(my_bsearch(&target, chars, 5, 1, cmp_char) == chars + 3);
        CHECK(my_bsearch(&miss_c, chars, 5, 1, cmp_char) == NULL);
    }
#endif
#ifdef T_RAND
    {
        int seq1[16], seq2[16];
        my_srand(42);
        int all_same = 1;
        for (int i = 0; i < 16; i++) {
            seq1[i] = my_rand();
            CHECK(seq1[i] >= 0 && seq1[i] <= 32767);
            if (i > 0 && seq1[i] != seq1[0]) all_same = 0;
        }
        CHECK(!all_same); /* must not return a constant */

        /* Re-seeding with 42 reproduces exact sequence */
        my_srand(42);
        for (int i = 0; i < 16; i++) CHECK(my_rand() == seq1[i]);

        /* Different seed produces a different sequence */
        my_srand(99);
        int diff = 0;
        for (int i = 0; i < 16; i++) {
            seq2[i] = my_rand();
            if (seq2[i] != seq1[i]) diff = 1;
        }
        CHECK(diff);
    }
#endif
}

/* ===================== allocator ===================== */

void test_malloc(void)
{
#ifdef T_MALLOC
    {
        CHECK(my_malloc(0) == NULL);
        my_free(NULL); /* must not crash */

        /* 1. Alignment trap: odd sizes (1, 3, 5, 7, 13) must ALL return 8-byte aligned ptrs */
        size_t odd_sizes[] = {1, 3, 5, 7, 13, 1, 9, 2};
        void *odd_ptrs[8];
        for (int i = 0; i < 8; i++) {
            odd_ptrs[i] = my_malloc(odd_sizes[i]);
            CHECK(odd_ptrs[i] != NULL);
            CHECK(((uintptr_t)odd_ptrs[i] % 8) == 0);
            memset(odd_ptrs[i], 0xA0 + i, odd_sizes[i]);
        }
        for (int i = 0; i < 8; i++) {
            unsigned char *p = odd_ptrs[i];
            int ok = 1;
            for (size_t j = 0; j < odd_sizes[i]; j++) ok &= (p[j] == (unsigned char)(0xA0 + i));
            CHECK(ok);
            my_free(odd_ptrs[i]);
        }

        /* 2. Payload isolation & 3-way coalescing (left + right + middle) */
        char *a = my_malloc(100), *b = my_malloc(200), *c = my_malloc(100);
        CHECK(a && b && c);
        memset(a, 'A', 100); memset(b, 'B', 200); memset(c, 'C', 100);
        my_free(a); /* free left */
        my_free(c); /* free right */
        int b_ok = 1;
        for (int i = 0; i < 200; i++) b_ok &= (b[i] == 'B');
        CHECK(b_ok); /* freeing neighbors must not corrupt b */
        my_free(b);  /* free middle -> should coalesce A + B + C */

        /* 3. Block splitting: large freed block split into smaller chunks */
        void *big = my_malloc(60000);
        CHECK(big != NULL);
        my_free(big);

        char *s1 = my_malloc(1000), *s2 = my_malloc(1000);
        CHECK(s1 && s2 && s1 != s2);
        memset(s1, 0x11, 1000);
        memset(s2, 0x22, 1000);
        CHECK((unsigned char)s1[999] == 0x11 && (unsigned char)s2[999] == 0x22);
        my_free(s1);
        my_free(s2);

        /* 4. Integer overflow / absurd size traps (n + header_size wraparound!) */
        CHECK(my_malloc((size_t)-1) == NULL);
        CHECK(my_malloc((size_t)-8) == NULL);
        CHECK(my_malloc((size_t)-64) == NULL);
        CHECK(my_malloc((size_t)1 << 30) == NULL);

        /* 5. Fragmentation & full recovery stress test */
        void *p[100];
        for (int i = 0; i < 100; i++) {
            size_t sz = (size_t)(i % 17) + 1;
            p[i] = my_malloc(sz);
            CHECK(p[i] != NULL);
            memset(p[i], (unsigned char)i, sz);
        }
        for (int i = 0; i < 100; i += 2) my_free(p[i]);
        for (int i = 1; i < 100; i += 2) my_free(p[i]);

        void *again = my_malloc(60000);
        CHECK(again != NULL); /* heap fully coalesced back */
        my_free(again);
    }
#endif
#ifdef T_CALLOC
    {
        /* Dirty the heap first so calloc cannot pass by relying on fresh .bss zeros! */
        void *dirty = my_malloc(256);
        if (dirty) {
            memset(dirty, 0xFF, 256);
            my_free(dirty);
        }

        unsigned char *z = my_calloc(64, 4);
        CHECK(z != NULL);
        int zero_ok = 1;
        for (int i = 0; z && i < 256; i++) zero_ok &= (z[i] == 0);
        CHECK(zero_ok);
        my_free(z);

        /* Multiplication overflow traps */
        CHECK(my_calloc((size_t)-1, 2) == NULL);
        CHECK(my_calloc(2, (size_t)-1) == NULL);
        size_t half_max = ((size_t)1 << (sizeof(size_t) * 4));
        CHECK(my_calloc(half_max, half_max) == NULL);

        /* Zero count or size */
        void *z0 = my_calloc(0, 10);
        my_free(z0);
        void *z1 = my_calloc(10, 0);
        my_free(z1);
    }
#endif
#ifdef T_REALLOC
    {
        /* 1. realloc(NULL, n) acts like malloc(n) */
        char *r = my_realloc(NULL, 32);
        CHECK(r != NULL && ((uintptr_t)r % 8) == 0);
        for (int i = 0; i < 32; i++) r[i] = (char)('a' + (i % 26));

        /* 2. Grow while another block sits right after it (forces move or coalesce) */
        char *blocker = my_malloc(64);
        CHECK(blocker != NULL);
        memset(blocker, 'Z', 64);

        r = my_realloc(r, 2000);
        CHECK(r != NULL);
        int data_ok = 1;
        for (int i = 0; i < 32; i++) data_ok &= (r[i] == (char)('a' + (i % 26)));
        CHECK(data_ok);

        /* Verify blocker wasn't corrupted */
        int blocker_ok = 1;
        for (int i = 0; i < 64; i++) blocker_ok &= (blocker[i] == 'Z');
        CHECK(blocker_ok);
        my_free(blocker);

        /* 3. Shrink in-place / copy */
        r = my_realloc(r, 16);
        CHECK(r != NULL);
        data_ok = 1;
        for (int i = 0; i < 16; i++) data_ok &= (r[i] == (char)('a' + (i % 26)));
        CHECK(data_ok);

        /* 4. Failed realloc MUST return NULL and leave original block valid */
        CHECK(my_realloc(r, (size_t)-1) == NULL);
        CHECK(r[0] == 'a' && r[15] == 'p');

        /* 5. realloc(p, 0) frees block and returns NULL */
        CHECK(my_realloc(r, 0) == NULL);

        /* Verify heap is completely healthy after all reallocs */
        void *all = my_malloc(60000);
        CHECK(all != NULL);
        my_free(all);
    }
#endif
}

/* ===================== math ===================== */

#if defined(T_FABS) || defined(T_FLOOR) || defined(T_SQRT) || defined(T_POW) || defined(T_FMOD)
int approx(double a, double b)
{
    double d = a > b ? a - b : b - a;
    double m = b < 0 ? -b : b;
    return d <= 1e-9 * (m > 1 ? m : 1);
}
#endif

void test_math(void)
{
    double v[] = {
        0.0, -0.0, 1.0, -1.0, 2.0, -2.0,
        0.5, -0.5, 2.5, -2.5,
        0.0001, -0.0001, 0.9999, -0.9999,
        2.999999, -2.999999, 2.000001, -2.000001,
        1e-6, -1e-6, 123456.789, -123456.789,
        123456789.0, -123456789.0
    };
    size_t nv = sizeof v / sizeof v[0];
    (void)v; (void)nv;

#ifdef T_FABS
    for (size_t i = 0; i < nv; i++) {
        CHECK(approx(my_fabs(v[i]), fabs(v[i])) && my_fabs(v[i]) >= 0.0);
    }
#endif
#ifdef T_FLOOR
    for (size_t i = 0; i < nv; i++) {
        if (!approx(my_floor(v[i]), floor(v[i]))) {
            failed++;
            printf("FAIL my_floor(%g): got %g, expected %g\n", v[i], my_floor(v[i]), floor(v[i]));
        } else {
            passed++;
        }
        if (!approx(my_ceil(v[i]), ceil(v[i]))) {
            failed++;
            printf("FAIL my_ceil(%g): got %g, expected %g\n", v[i], my_ceil(v[i]), ceil(v[i]));
        } else {
            passed++;
        }
    }
#endif
#ifdef T_SQRT
    {
        /* Includes 0 < x < 1 where sqrt(x) > x (breaks naive [0, x] binary search!) */
        double sq[] = {
            0.0, 1.0, 2.0, 3.0, 4.0, 9.0, 144.0,
            0.25, 0.01, 0.0001, 1e-8, 0.5,
            1e10, 1e12, 123456.789
        };
        for (size_t i = 0; i < sizeof sq / sizeof sq[0]; i++) {
            if (!approx(my_sqrt(sq[i]), sqrt(sq[i]))) {
                failed++;
                printf("FAIL my_sqrt(%g): got %.12g, expected %.12g\n",
                       sq[i], my_sqrt(sq[i]), sqrt(sq[i]));
            } else {
                passed++;
            }
        }
    }
#endif
#ifdef T_POW
    CHECK(approx(my_pow_int(2.0, 0), 1.0));
    CHECK(approx(my_pow_int(0.0, 0), 1.0));
    CHECK(approx(my_pow_int(-5.0, 0), 1.0));
    CHECK(approx(my_pow_int(0.0, 5), 0.0));
    CHECK(approx(my_pow_int(2.0, 1), 2.0));
    CHECK(approx(my_pow_int(2.0, 10), 1024.0));
    CHECK(approx(my_pow_int(2.0, -1), 0.5));
    CHECK(approx(my_pow_int(2.0, -2), 0.25));
    CHECK(approx(my_pow_int(0.5, -3), 8.0));
    CHECK(approx(my_pow_int(-2.0, 3), -8.0));
    CHECK(approx(my_pow_int(-2.0, 4), 16.0));
    CHECK(approx(my_pow_int(-2.0, -3), -0.125));
    CHECK(approx(my_pow_int(-2.0, -4), 0.0625));
    CHECK(approx(my_pow_int(1.0, 100000), 1.0));
    CHECK(approx(my_pow_int(-1.0, 100001), -1.0));
#endif
#ifdef T_FMOD
    {
        struct { double a, b; } fm[] = {
            {7.0, 3.0}, {-7.0, 3.0}, {7.0, -3.0}, {-7.0, -3.0}, /* all 4 sign combos */
            {6.0, 3.0}, {-6.0, 3.0}, {0.0, 5.0},                /* exact multiples */
            {7.5, 2.0}, {-7.5, 2.0}, {5.5, 1.25}, {-5.5, 1.25}, /* fractional */
            {1.0, 5.0}, {-1.0, 5.0}, {0.25, 0.5}                /* |a| < |b| */
        };
        for (size_t i = 0; i < sizeof fm / sizeof fm[0]; i++) {
            double got = my_fmod(fm[i].a, fm[i].b);
            double exp_val = fmod(fm[i].a, fm[i].b);
            if (!approx(got, exp_val)) {
                failed++;
                printf("FAIL my_fmod(%g, %g): got %g, expected %g\n",
                       fm[i].a, fm[i].b, got, exp_val);
            } else {
                passed++;
            }
        }
    }
#endif
}

/* ===================== output ===================== */

#ifdef T_OUTPUT
static char out_buf[128];
static size_t out_idx;

static void capture_putc(char c)
{
    if (out_idx + 1 < sizeof out_buf)
        out_buf[out_idx++] = c;
    out_buf[out_idx] = '\0';
}

static void reset_out(void)
{
    out_idx = 0;
    out_buf[0] = '\0';
}
#endif

void test_output(void)
{
#ifdef T_OUTPUT
    my_putc_hook = capture_putc;

    reset_out();
    my_puts("hello");
    /* Accepts either bare string or trailing newline like standard puts() */
    CHECK(strcmp(out_buf, "hello") == 0 || strcmp(out_buf, "hello\n") == 0);

    long nums[] = {0, 1, -1, 42, -42, 123456789L, LONG_MAX, LONG_MIN};
    for (size_t i = 0; i < sizeof nums / sizeof nums[0]; i++) {
        char exp_str[64];
        snprintf(exp_str, sizeof exp_str, "%ld", nums[i]);
        reset_out();
        my_putint(nums[i]);
        CHECK(strcmp(out_buf, exp_str) == 0);
    }
#endif
}

/* ===================== main ===================== */

int main(void)
{
    test_ctype();
    test_mem();
    test_string();
    test_convert();
    test_sort();
    test_malloc();
    test_math();
    test_output();

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed != 0;
}
