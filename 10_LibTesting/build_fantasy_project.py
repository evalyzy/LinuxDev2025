import os
import sys

def write_file(path, content):
    """Writes content to a file, creating directories if needed."""
    directory = os.path.dirname(path)
    if directory and not os.path.exists(directory):
        os.makedirs(directory)
    with open(path, "w") as f:
        f.write(content)
    print(f"Created: {path}")

# ==========================================
# 1. Root Level Files
# ==========================================

configure_ac = r"""#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([namegen], [0.0.1], [george@altlinux.org])
AM_INIT_AUTOMAKE([foreign])
LT_INIT([disable-static])
AC_CONFIG_SRCDIR([src/namegen.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Joint pkgconfig library/include check and variable definition.
PKG_CHECK_MODULES([CHECK],[check])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strchr])

AC_CONFIG_FILES([Makefile
                 tests/Makefile
                 src/Makefile])
AC_OUTPUT
"""

makefile_am = r"""SUBDIRS = src tests

demo:	all
	./src/namegen '!BVCs !DdM'
"""

# ==========================================
# 2. src Directory Files
# ==========================================

src_makefile_am = r"""CFLAGS  = -ansi -pedantic -Wall -Wextra -O3 -g3

lib_LTLIBRARIES = libnamegen.la
bin_PROGRAMS = namegen

namegen_LDADD = libnamegen.la

libnamegen.c: const.h

const.h: tablegen.py
	python3 $< > $@

check: namegen
	@./namegen --test
"""

# src/namegen.h (Header)
src_namegen_h = r"""/* Fantasy name generator ANSI C header library
 * This is free and unencumbered software released into the public domain.
 */
#ifndef NAMEGEN_H
#define NAMEGEN_H

#define NAMEGEN_MAX_DEPTH  32 /* Cannot exceed bits in a long */

/* Return codes */
#define NAMEGEN_SUCCESS    0
#define NAMEGEN_TRUNCATED  1  /* Output was truncated */
#define NAMEGEN_INVALID    2  /* Pattern is invalid */
#define NAMEGEN_TOO_DEEP   3  /* Pattern exceeds maximum nesting depth */

int namegen(char *, unsigned long, const char *, unsigned long *);

#endif
"""

# src/libnamegen.c (Library Implementation)
src_libnamegen_c = r"""/* Fantasy name generator ANSI C header library
 * This is free and unencumbered software released into the public domain.
 *
 * This library is based on the RinkWorks Fantasy Name Generator.
 * http://www.rinkworks.com/namegen/
 */
#include "namegen.h"
#include "const.h"
#include <stddef.h> 

/* Implementation */

static int
namegen_special(int c)
{
    if (c >= 0 && c < 128)
        return special[c];
    return -1;
}

static int
namegen_offsets(int n, const short **offsets)
{
    *offsets = offsets_table + off_len[n * 2 + 0];
    return off_len[n * 2 + 1];
}

static unsigned long
namegen_rand32(unsigned long *s)
{
    unsigned long x = *s;
    x ^= x << 13;
    x ^= (x & 0xffffffffUL) >> 17;
    x ^= x << 5;
    return (*s = x) & 0xffffffffUL;
}

static int
namegen_cap(int c, int capitalize)
{
    return capitalize && c >= 'a' && c <= 'z' ? c & ~0x20 : c;
}

static char *
namegen_copy(char *p, char *e, int c, unsigned long *seed, int capitalize)
{
    int n = namegen_special(c);
    if (n == -1) {
        if (p != e)
            *p++ = namegen_cap(c, capitalize);
    } else {
        const short *offsets;
        int count = namegen_offsets(n, &offsets);
        int select = namegen_rand32(seed) % count;
        const char *s = namegen_argz + offsets[select];
        while (*s) {
            int r = *s++;
            if (p != e)
                *p++ = namegen_cap(r, capitalize);
            capitalize = 0;
        }
    }
    return p;
}

int
namegen(char *dst, unsigned long len, const char *pattern, unsigned long *seed)
{
    int depth = 0;               /* Current nesting depth */
    char *p = dst;               /* Current output pointer */
    char *e = dst + len;         /* Maxiumum output pointer */
    int capitalize = 0;          /* Capitalize next item */

    /* Stacks */
    char *reset[NAMEGEN_MAX_DEPTH];     /* Reset pointer (undo generate) */
    unsigned long n[NAMEGEN_MAX_DEPTH]; /* Number of groups */
    unsigned long silent = 0;    /* Actively generating? */
    unsigned long literal = 0;   /* Current "mode" */
    unsigned long capstack = 0;  /* Initial capitalization state */

    n[0] = 1;
    reset[0] = dst;
    for (; *pattern; pattern++) {
        unsigned long bit; /* Bit for current depth */
        int c = *pattern;
        switch (c) {
            case '<':
                if (++depth == NAMEGEN_MAX_DEPTH) {
                    *dst = 0;
                    return NAMEGEN_TOO_DEEP;
                }
                bit = 1UL << depth;
                n[depth] = 1;
                reset[depth] = p;
                literal &= ~bit;
                silent &= ~bit;
                silent |= (silent << 1) & bit;
                capstack &= ~bit;
                capstack |= (unsigned long)capitalize << depth;
                break;

            case '(':
                if (++depth == NAMEGEN_MAX_DEPTH) {
                    *dst = 0;
                    return NAMEGEN_TOO_DEEP;
                }
                bit = 1UL << depth;
                n[depth] = 1;
                reset[depth] = p;
                literal |= bit;
                silent &= ~bit;
                silent |= (silent << 1) & bit;
                capstack &= ~bit;
                capstack |= (unsigned long)capitalize << depth;
                break;

            case '>':
                if (depth == 0) {
                    *dst = 0;
                    return NAMEGEN_INVALID;
                }
                bit = 1UL << depth--;
                if (literal & bit) {
                    *dst = 0;
                    return NAMEGEN_INVALID;
                }
                break;

            case ')':
                if (depth == 0) {
                    *dst = 0;
                    return NAMEGEN_INVALID;
                }
                bit = 1UL << depth--;
                if (!(literal & bit)) {
                    *dst = 0;
                    return NAMEGEN_INVALID;
                }
                break;

            case '|':
                bit = 1UL << depth;
                /* Stay silent if parent group is silent */
                if (!(silent & (bit >> 1))) {
                    if (namegen_rand32(seed) < 0xffffffffUL / ++n[depth]) {
                        /* Switch to this option */
                        p = reset[depth];
                        silent &= ~bit;
                        capitalize = !!(capstack & bit);\
                    } else {
                        /* Skip this option */
                        silent |= bit;
                    }
                }
                break;

            case '!':
                capitalize = 1;
                break;

            default:
                bit = 1UL << depth;
                if (!(silent & bit)) {
                    if (literal & bit) {
                        /* Copy value literally */
                        if (p != e)
                            *p++ = namegen_cap(c, capitalize);
                    } else {
                        /* Copy a substitution */\
                        p = namegen_copy(p, e, c, seed, capitalize);
                    }
                }
                capitalize = 0;
        }
    }

    if (depth) {
        *dst = 0;
        return NAMEGEN_INVALID;
    } else if (p == e) {
        p[-1] = 0;
        return NAMEGEN_TRUNCATED;
    } else {
        *p = 0;
        return NAMEGEN_SUCCESS;
    }
}
"""

# src/namegen.c (Main Program)
src_namegen_c = r"""#include "config.h"
#include "namegen.h"
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static unsigned long
hash32(unsigned long a)
{
    a = a & 0xffffffffUL;
    a = (a ^ 61UL) ^ (a >> 16);
    a = (a + (a << 3)) & 0xffffffffUL;
    a = a ^ (a >> 4);
    a = (a * 0x27d4eb2dUL) & 0xffffffffUL;
    a = a ^ (a >> 15);
    return a;
}

int
main(int argc, char **argv)
{
    int i;
    int count = 1;
    FILE *urandom;
    unsigned char randbuf[4];
    unsigned long seed[] = {0x8af611acUL};

    /* Parse command line arguments */
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <pattern> [num]\n", argv[0]);
        printf("  pattern   template for names to generate\n");
        printf("  num       number of names to generate\n");
        exit(EXIT_FAILURE);
    } else if (argc == 3) {
        count = atoi(argv[2]);
    }

    /* Shuffle up the seed a bit */
    urandom = fopen("/dev/urandom", "rb");
    if (urandom && fread(randbuf, sizeof(randbuf), 1, urandom)) {
        unsigned long rnd =
            (unsigned long)randbuf[0] <<  0 |
            (unsigned long)randbuf[1] <<  8 |
            (unsigned long)randbuf[2] << 16 |
            (unsigned long)randbuf[3] << 24;
        *seed ^= rnd;
        fclose(urandom);
    } else {
        void *p = malloc(4UL * 1024 * 1024);
        *seed ^= hash32(time(0));             /* Current time */
        *seed ^= hash32((ptrdiff_t)main);     /* ASLR entopy */
        *seed ^= hash32((ptrdiff_t)seed);     /* Stack gap entropy */
        *seed ^= hash32((ptrdiff_t)p);        /* Allocator entropy */
        *seed ^= hash32((ptrdiff_t)malloc);   /* C library ASLR */
        free(p);
    }

    /* Generate some names */
    for (i = 0; i < count; i++) {
        char buf[256];
        int r = namegen(buf, sizeof(buf), argv[1], seed);
        if (r == NAMEGEN_INVALID) {
            fprintf(stderr, "%s: invalid pattern\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        printf("%s\n", buf);
    }
    return 0;
}
"""

# src/tablegen.py (Data Generator)
src_tablegen_py = r"""
table = {
    's': ["ach", "ack", "ad", "age", "ald", "ale", "an", "ang", "ar", "ard",
          "as", "ash", "at", "ath", "augh", "aw", "ban", "bel", "bur", "cer",
          "cha", "che", "dan", "dar", "del", "den", "dra", "dyn", "ech", "eld",
          "elm", "em", "en", "end", "eng", "enth", "er", "ess", "est", "et",
          "gar", "gha", "hat", "hin", "hon", "ia", "ight", "ild", "im", "ina",
          "ine", "ing", "ir", "is", "iss", "it", "kal", "kel", "kim", "kin",
          "ler", "lor", "lye", "mor", "mos", "nal", "ny", "nys", "old", "om",
          "on", "or", "orm", "os", "ough", "per", "pol", "qua", "que", "rad",
          "rak", "ran", "ray", "ril", "ris", "rod", "roth", "ryn", "sam",
          "say", "ser", "shy", "skel", "sul", "tai", "tan", "tas", "ther",
          "tia", "tin", "ton", "tor", "tur", "um", "und", "unt", "urn", "usk",
          "ust", "ver", "ves", "vor", "war", "wor", "yer"],
    'v': ["a", "e", "i", "o", "u", "y"],
    'V': ["a", "e", "i", "o", "u", "y", "ae", "ai", "au", "ay", "ea", "ee",
          "ei", "eu", "ey", "ia", "ie", "oe", "oi", "oo", "ou", "ui"],
    'c': ["b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r",
          "s", "t", "v", "w", "x", "y", "z"],
    'B': ["b", "bl", "br", "c", "ch", "chr", "cl", "cr", "d", "dr", "f", "g",
          "h", "j", "k", "l", "ll", "m", "n", "p", "ph", "qu", "r", "rh", "s",
          "sch", "sh", "sl", "sm", "sn", "st", "str", "sw", "t", "th", "thr",
          "tr", "v", "w", "wh", "y", "z", "zh"],
    'C': ["b", "c", "ch", "ck", "d", "f", "g", "gh", "h", "k", "l", "ld", "ll",
          "lt", "m", "n", "nd", "nn", "nt", "p", "ph", "q", "r", "rd", "rr",
          "rt", "s", "sh", "ss", "st", "t", "th", "v", "w", "y", "z"],
    'i': ["elch", "idiot", "ob", "og", "ok", "olph", "olt", "omph", "ong",
          "onk", "oo", "oob", "oof", "oog", "ook", "ooz", "org", "ork", "orm",
          "oron", "ub", "uck", "ug", "ulf", "ult", "um", "umb", "ump", "umph",
          "un", "unb", "ung", "unk", "unph", "unt", "uzz"],
    'm': ["baby", "booble", "bunker", "cuddle", "cuddly", "cutie", "doodle",
          "foofie", "gooble", "honey", "kissie", "lover", "lovey", "moofie",
          "mooglie", "moopie", "moopsie", "nookum", "poochie", "poof",
          "poofie", "pookie", "schmoopie", "schnoogle", "schnookie",
          "schnookum", "smooch", "smoochie", "smoosh", "snoogle", "snoogy",
          "snookie", "snookum", "snuggy", "sweetie", "woogle", "woogy",
          "wookie", "wookum", "wuddle", "wuddly", "wuggy", "wunny"],
    'M': ["boo", "bunch", "bunny", "cake", "cakes", "cute", "darling",
          "dumpling", "dumplings", "face", "foof", "goo", "head", "kin",
          "kins", "lips", "love", "mush", "pie", "poo", "pooh", "pook",
          "pums"],
    'D': ["b", "bl", "br", "cl", "d", "f", "fl", "fr", "g", "gh", "gl", "gr",
          "h", "j", "k", "kl", "m", "n", "p", "th", "w"],
    'd': ["air", "ankle", "ball", "beef", "bone", "bum", "bumble", "bump",
          "cheese", "clod", "clot", "clown", "corn", "dip", "dolt", "doof",
          "dork", "dumb", "face", "finger", "foot", "fumble", "goof",
          "grumble", "head", "knock", "knocker", "knuckle", "loaf", "lump",
          "lunk", "meat", "muck", "munch", "nit", "numb", "pin", "puff",
          "skull", "snark", "sneeze", "thimble", "twerp", "twit", "wad",
          "wimp", "wipe"]
}

order = "svVcBCimMDd"

argz = []
offsets = []
for c in order:
    items = table[c]
    offsets.append(len(argz))
    for item in items:
        argz.extend(map(ord, item))
        argz.append(0)

print("static const signed char special[] = {")
for i in range(128):
    if i % 8 == 0:
        print("    ", end="")
    if chr(i) in order:
        print(f"0x{order.index(chr(i)):02x}, ", end="")
    else:
        print("  -1, ", end="")
    if i % 8 == 7:
        print()
print("};")

print("static const short offsets_table[] = {")
for i, x in enumerate(argz):
    if i % 8 == 0:
        print("    ", end="")
    print(f"0x{x:04x}, ", end="")
    if i % 8 == 7:
        print()
print("\n};")

print("static const short off_len[] = {")
for i in range(len(offsets)):
    if i % 8 == 0:
        print("    ", end="")
    print(f"0x{offsets[i]:04x}, 0x{len(table[order[i]]):04x}, ", end="")
    if i % 8 == 7:
        print()
print("\n};")

print("static const char namegen_argz[] = {")
for i, x in enumerate(argz):
    if i % 15 == 0:
        print("    ", end="")
    if x:
        print(f"'{chr(x)}', ", end="")
    else:
        print(" 0 , ", end="")
    if i % 15 == 14:
        print()
print("\n};")
"""

# ==========================================
# 3. tests Directory Files
# ==========================================

tests_makefile_am = r"""TESTS = dummy upstream
check_PROGRAMS = dummy upstream

.ts.c:
	checkmk $< > $@

AM_CFLAGS = -I$(top_builddir)/src @CHECK_CFLAGS@
LDADD = $(top_builddir)/src/libnamegen.la @CHECK_LIBS@
"""

tests_dummy_ts = r"""#include <check.h>
#include "namegen.h"

#test dummy
	ck_assert_ptr_nonnull(namegen);
"""

tests_upstream_ts = r"""#include <check.h>
#include "namegen.h"

int r;
char buf[256];
int count_pass = 0;
int count_fail = 0;
unsigned long rng[] = {0xd22bfb3b};

#suite Tools
#tcase Alternation
#test alternation_test
        int i;
        int mismatch = 0;
        int seen[3] = {0, 0, 0};
        for (i = 0; i < 100; i++) {
            memset(buf, 0x5a, sizeof(buf));
            r = namegen(buf, sizeof(buf), "0|1|2", rng);
            if (r != NAMEGEN_SUCCESS || buf[1] || !strchr("012", buf[0])) {
                mismatch++;
            } else {
                seen[buf[0] - '0']++;
            }
        }
	ck_assert_int_eq(mismatch, 0);
	ck_assert_int_ne(seen[0], 0);
	ck_assert_int_ne(seen[1], 0);
	ck_assert_int_ne(seen[2], 0);

#tcase Substitution
#test substitution_test
        int i;
        int mismatch = 0;
        int seen[6] = {0, 0, 0};
        for (i = 0; i < 100; i++) {
            memset(buf, 0x5a, sizeof(buf));
            r = namegen(buf, sizeof(buf), "(v)v", rng);
            if (r != NAMEGEN_SUCCESS || buf[0] != 'v' || !buf[1] || buf[2]) {
                mismatch++;
            } else {
                char *p = "aeiouy";
                char *s = strchr(p, buf[1]);
                if (!s)
                    mismatch++;
                else
                    seen[s - p]++;
            }
        }
        for (i = 0; i < 6; i++)
            mismatch += !seen[i];
	ck_assert_int_eq(mismatch, 0);
	
#suite Errors
#tcase bracket
#test closing_bracket_test
        r = namegen(buf, sizeof(buf), "a>", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#test mismatch_bracket_test
        r = namegen(buf, sizeof(buf), "(a>", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#test unclosed_bracket_test
        r = namegen(buf, sizeof(buf), "<a", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#tcase paren
#test closing_paren_test
        r = namegen(buf, sizeof(buf), "a)", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#test mismatch_paren_test
        r = namegen(buf, sizeof(buf), "<a)", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#test unclosed_paren_test
        r = namegen(buf, sizeof(buf), "(a", rng);
	ck_assert_int_eq(r, NAMEGEN_INVALID);

#suite Tools2
#tcase Deep_nesting
#test deep_nesting_test
        char *p = "(((((<(((((((((((((((a)))))))))))))))>)))))\";
        memset(buf, 0x5a, sizeof(buf));
        r = namegen(buf, sizeof(buf), p, rng);
	ck_assert_int_eq(r, NAMEGEN_SUCCESS);
	ck_assert_str_eq(buf, "a");

#tcase Truncation
#test truncation_1_test
        memset(buf, 0x5a, sizeof(buf));
        r = namegen(buf, 4, "(abcdefg)", rng);
	ck_assert_int_eq(r, NAMEGEN_TRUNCATED);
	ck_assert_str_eq(buf, "abc");
	ck_assert_uint_eq(buf[4], 0x5a);

#test truncation_2_test
        memset(buf, 0x5a, sizeof(buf));
        r = namegen(buf, 2, "i", rng);
	ck_assert_int_eq(r, NAMEGEN_TRUNCATED);
	ck_assert_uint_ne(buf[0], 0);
	ck_assert_uint_eq(buf[2], 0x5a);

#tcase Capitalization
#test capitalization_1_test
        r = namegen(buf, sizeof(buf), "!(foo)", rng);
	ck_assert_int_eq(r, NAMEGEN_SUCCESS);
	ck_assert_str_eq(buf, "Foo");

#test capitalization_2_test
        r = namegen(buf, sizeof(buf), "(!foo)", rng);
	ck_assert_int_eq(r, NAMEGEN_SUCCESS);
	ck_assert_str_eq(buf, "Foo");

#test capitalization_3_test
        r = namegen(buf, sizeof(buf), "(f!oo)", rng);
	ck_assert_int_eq(r, NAMEGEN_SUCCESS);
	ck_assert_str_eq(buf, "fOo");

#tcase Empty
#test epmty_test
        memset(buf, 0x5a, sizeof(buf));
        r = namegen(buf, sizeof(buf), "", rng);
	ck_assert_int_eq(r, NAMEGEN_SUCCESS);
	ck_assert_uint_eq(buf[0], 0);
	ck_assert_uint_eq(buf[1], 0x5a);
"""


# ==========================================
# 4. Writing Files
# ==========================================

# Create directories
os.makedirs("src", exist_ok=True)
os.makedirs("tests", exist_ok=True)

# Write root files
write_file("configure.ac", configure_ac)
write_file("Makefile.am", makefile_am)

# Write src files
write_file("src/Makefile.am", src_makefile_am)
write_file("src/namegen.h", src_namegen_h)
write_file("src/libnamegen.c", src_libnamegen_c)
write_file("src/namegen.c", src_namegen_c)
write_file("src/tablegen.py", src_tablegen_py)

# Write tests files
write_file("tests/Makefile.am", tests_makefile_am)
write_file("tests/dummy.ts", tests_dummy_ts)
write_file("tests/upstream.ts", tests_upstream_ts)

print("\nAll files created successfully!")
print("\nNow run the following commands to build and test:")
print("1. autoreconf -fisv")
print("2. ./configure")
print("3. make")
print("4. make check")

