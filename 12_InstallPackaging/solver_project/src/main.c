#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include "config.h"
#include "solver.h"

#define _(STRING) gettext(STRING)
#define N_(STRING) gettext(STRING)

int main(int argc, char *argv[]) {
    // 初始化本地化
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    if (argc < 4) {
        printf(_("Usage: %s <a> <b> <c>\n"), argv[0]);
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    double c = atof(argv[3]);
    double x1, x2;

    int n = solve_quadratic(a, b, c, &x1, &x2);

    if (n == -1) printf(_("Infinite roots\n"));
    else if (n == 0) printf(_("No real roots\n"));
    else if (n == 1) printf(_("One root: %f\n"), x1);
    else printf(_("Two roots: %f and %f\n"), x1, x2);

    return 0;
}
