/** @file guess.c
 * @brief Number guessing game with Roman numeral support.
 * @date 2025-12-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include "config.h" 

#define _(STRING) gettext(STRING)
#define N_ 100

/* 罗马数字表 (1-100) */
static const char *roman_numerals[N_ + 1] = {
    "N/A", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
    "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
    "XXI", "XXII", "XXIII", "XXIV", "XXV", "XXVI", "XXVII", "XXVIII", "XXIX", "XXX",
    "XXXI", "XXXII", "XXXIII", "XXXIV", "XXXV", "XXXVI", "XXXVII", "XXXVIII", "XXXIX", "XL",
    "XLI", "XLII", "XLIII", "XLIV", "XLV", "XLVI", "XLVII", "XLVIII", "XLIX", "L",
    "LI", "LII", "LIII", "LIV", "LV", "LVI", "LVII", "LVIII", "LIX", "LX",
    "LXI", "LXII", "LXIII", "LXIV", "LXV", "LXVI", "LXVII", "LXVIII", "LXIX", "LXX",
    "LXXI", "LXXII", "LXXIII", "LXXIV", "LXXV", "LXXVI", "LXXVII", "LXXVIII", "LXXIX", "LXXX",
    "LXXXI", "LXXXII", "LXXXIII", "LXXXIV", "LXXXV", "LXXXVI", "LXXXVII", "LXXXVIII", "LXXXIX", "XC",
    "XCI", "XCII", "XCIII", "XCIV", "XCV", "XCVI", "XCVII", "XCVIII", "XCIX", "C"
};

/**
 * @brief Convert integer to Roman numeral.
 * @param n Integer (1-100)
 * @return Roman string or "?"
 */
const char* to_roman(int n) {
    if (n >= 1 && n <= N_) return roman_numerals[n];
    return "?";
}

/**
 * @brief Format number string based on mode.
 * @param n Number to format
 * @param use_roman 1 for Roman, 0 for Arabic
 * @param buf Output buffer
 */
char* format_num(int n, int use_roman, char *buf) {
    if (use_roman) {
        strcpy(buf, to_roman(n));
    } else {
        sprintf(buf, "%d", n);
    }
    return buf;
}

/**
 * @brief Print help message.
 * Used for --help and generating man page content.
 */
void print_help() {
    printf(_("Usage: guess [OPTIONS]\n"));
    printf(_("Guess the number game.\n\n"));
    printf(_("  -r         Use Roman numerals (I-C)\n"));
    printf(_("  --help     Display this help and exit\n"));
    printf(_("  --version  Output version information and exit\n"));
}

/** @page guess Guess Game
 * @section SYNOPSIS
 * `guess` [ \a -r ]
 * @section DESCRIPTION
 * The computer tries to guess a number you are thinking of (1-100).
 * @section OPTIONS
 * - `-r`: Enable Roman numeral mode.
 */

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALEDIR);
    textdomain("guess");

    int use_roman = 0;

    for(int i=1; i<argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            use_roman = 1;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("guess 1.0\n");
            return 0;
        }
    }

    printf(_("Think of a number between 1 and %d.\n"), N_);
    
    int low = 1, high = N_;
    char buf[16];

    while (low <= high) {
        int mid = (low + high) / 2;
        printf(_("Is your number greater than %s? (y/n): "), format_num(mid, use_roman, buf));
        
        char answer[10];
        if (scanf("%9s", answer) != 1) break;

        if (answer[0] == 'y' || answer[0] == 'Y') {
            low = mid + 1;
        } else {
            high = mid;
        }

        if (low == high) {
            printf(_("Your number is %s!\n"), format_num(low, use_roman, buf));
            break;
        }
    }

    return 0;
}

