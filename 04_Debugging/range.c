#include <stdio.h>
#include <stdlib.h>

/* 打印用法说明（与原始输出一致） */
static void help(void) {
    fprintf(stderr,
        "Usage:\n"
        "  range N         -> outputs 0..N-1\n"
        "  range M N       -> outputs M..N-1\n"
        "  range M N S     -> outputs M, M+S, ... until N-1 (exclusive)\n"
    );
}

/* GDB 断点占位：保持签名与副作用不变 */
int should_print(int start, int stop, int step, int current, int idx) {
    (void)start; (void)stop; (void)step; (void)current; (void)idx;
    return 0;
}

int main(int argc, char *argv[]) {
    int start = 0, stop = 0, step = 1;
    int current;
    int idx = 0;

    /* 参数解析：保留与原程序相同的分支与默认值 */
    switch (argc) {
        case 1:
            help();
            return 1;
        case 2:
            start = 0;
            stop = atoi(argv[1]);
            step = 1;
            break;
        case 3:
            start = atoi(argv[1]);
            stop = atoi(argv[2]);
            step = 1;
            break;
        case 4:
            start = atoi(argv[1]);
            stop = atoi(argv[2]);
            step = atoi(argv[3]);
            break;
        default:
            help();
            return 1;
    }

    if (step == 0) {
        fprintf(stderr, "step must not be 0\n");
        return 2;
    }

    /* 生成输出：严格保留 <stop 与 >stop 的终止条件以及步进方向 */
    if (step > 0) {
        for (idx = 0, current = start; current < stop; current += step, idx++) {
            should_print(start, stop, step, current, idx);
            printf("%d\n", current);
        }
    } else {
        for (idx = 0, current = start; current > stop; current += step, idx++) {
            should_print(start, stop, step, current, idx);
            printf("%d\n", current);
        }
    }

    return 0;
}

