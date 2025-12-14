#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <errno.h>

// 定义原始 unlink 函数的指针类型
typedef int (*unlink_type)(const char *);

int unlink(const char *pathname) {
    // 检查文件名是否包含 "PROTECT"
    if (strstr(pathname, "PROTECT") != NULL) {
        // 如果包含，则禁止删除
        // 我们可以选择返回成功(0)以此来"欺骗"程序说删除了，
        // 或者返回错误(EACCES)。这里选择打印信息并返回错误，更符合逻辑。
        fprintf(stderr, "PROTECT: prevented deletion of %s\n", pathname);
        errno = EACCES; // Permission denied
        return -1;
    }

    // 获取原始的 unlink 函数
    unlink_type original_unlink = (unlink_type)dlsym(RTLD_NEXT, "unlink");
    
    if (!original_unlink) {
        fprintf(stderr, "Error getting original unlink symbol\n");
        return -1;
    }

    // 调用原始函数
    return original_unlink(pathname);
}

