#include <stdio.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include "config.h"

#define _(STRING) gettext(STRING)

int main() {
    // 初始化本地化环境
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    int low = 1;
    int high = 100;
    char answer[100];

    printf(_("Please simplify pick a number between 1 and 100.\n"));
    printf(_("Press Enter when ready..."));
    getchar(); // 等待用户按回车

    while (low < high) {
        int mid = (low + high) / 2;
        printf(_("Is the number greater than %d? (yes/no): "), mid);
        
        if (scanf("%99s", answer) != 1) {
            break;
        }

        // 简单的字符串匹配，支持 y/yes 和 n/no
        if (strcasecmp(answer, _("yes")) == 0 || strcasecmp(answer, "y") == 0) {
            low = mid + 1;
        } else if (strcasecmp(answer, _("no")) == 0 || strcasecmp(answer, "n") == 0) {
            high = mid;
        } else {
            printf(_("Please answer 'yes' or 'no'.\n"));
        }
    }

    printf(_("Your number is %d!\n"), low);

    return 0;
}

