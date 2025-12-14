#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_GROUPS 10 // 0 (whole match) + 1-9 groups

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <regexp> <substitution> <string>\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    const char *subst = argv[2];
    const char *str = argv[3];

    regex_t regex;
    int ret;

    // 1. Compile Regex
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        char errbuf[256];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex compilation error: %s\n", errbuf);
        return 1;
    }

    // 2. Execute Regex
    regmatch_t pmatch[MAX_GROUPS];
    ret = regexec(&regex, str, MAX_GROUPS, pmatch, 0);

    if (ret == REG_NOMATCH) {
        // No match, print original string and exit
        printf("%s\n", str);
        regfree(&regex);
        return 0;
    } else if (ret) {
        char errbuf[256];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex execution error: %s\n", errbuf);
        regfree(&regex);
        return 1;
    }

    // 3. Perform Substitution
    // Print part before the match
    printf("%.*s", (int)pmatch[0].rm_so, str);

    // Parse substitution string
    const char *p = subst;
    while (*p) {
        if (*p == '\\') {
            p++;
            if (*p >= '0' && *p <= '9') {
                int group_idx = *p - '0';
                // Check if this group was captured
                // Note: pmatch[group_idx].rm_so == -1 means group didn't participate in match
                // But task says "reference to non-existing pockets (e.g. \9 if no such brackets) is error"
                // regex.re_nsub tells how many subexpressions are in pattern.
                
                if (group_idx > (int)regex.re_nsub) {
                     fprintf(stderr, "Error: Reference to non-existent group \\%d\n", group_idx);
                     regfree(&regex);
                     return 1;
                }
                
                if (pmatch[group_idx].rm_so == -1) {
                    // Group exists in regex but didn't match anything (e.g. (a)? and it wasn't there)
                    // Usually we print nothing or error. Task implies error on "non-existing pockets".
                    // Let's assume structural non-existence is the main error. 
                    // If structurally exists but empty match, we print nothing.
                } else {
                    printf("%.*s", (int)(pmatch[group_idx].rm_eo - pmatch[group_idx].rm_so), 
                           str + pmatch[group_idx].rm_so);
                }
            } else if (*p == '\\') {
                putchar('\\');
            } else {
                // Handle single backslash or other escapes if needed. 
                // For now, print backslash and char literaly or ignore backslash?
                // Task says handle "\\" -> "\". 
                // Standard behavior for undefined escapes is often just the char.
                // But let's act safe:
                if (*p == '\0') {
                    // Trailing backslash
                    putchar('\\');
                    break;
                }
                putchar(*p); // Treat "\a" as "a"
            }
        } else {
            putchar(*p);
        }
        if (*p) p++;
    }

    // Print part after the match
    printf("%s\n", str + pmatch[0].rm_eo);

    regfree(&regex);
    return 0;
}

