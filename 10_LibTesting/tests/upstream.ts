#include <check.h>
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
        char *p = "(((((<(((((((((((((((a)))))))))))))))>)))))";
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
