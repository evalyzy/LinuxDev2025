#define _GNU_SOURCE
#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();

    // 获取屏幕大小并创建带边框的窗口
    int rows = LINES - 2, cols = COLS - 2;
    WINDOW *frame = newwin(rows, cols, 1, 1);
    box(frame, 0, 0);
    mvwprintw(frame, 0, 2, " %s ", argv[1]);
    wrefresh(frame);

    // 内容子窗口
    WINDOW *win = newwin(rows - 2, cols - 2, 2, 2);
    scrollok(win, TRUE);
    keypad(win, TRUE);

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        endwin();
        perror("fopen");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    int ch;
    // 首次填充一屏
    for (int i = 0; i < rows - 2 && getline(&line, &len, f) > 0; i++) {
        wprintw(win, "%.*s\n", cols - 2, line);
    }
    wrefresh(win);

    // 交互：空格滚动一行，ESC 退出
    while ((ch = wgetch(win)) != 27) {
        if (ch == ' ') {
            if (getline(&line, &len, f) > 0) {
                wprintw(win, "%.*s\n", cols - 2, line);
                wrefresh(win);
            }
        }
    }

    free(line);
    fclose(f);
    delwin(win);
    delwin(frame);
    endwin();
    return 0;
}

