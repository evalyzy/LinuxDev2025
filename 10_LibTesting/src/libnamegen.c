/* Fantasy name generator ANSI C header library
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
