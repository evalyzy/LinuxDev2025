#define _GNU_SOURCE
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <rhash.h>

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* 辅助函数：根据名称获取 Hash ID */
unsigned get_hash_id(const char *name) {
    if (strcasecmp(name, "md5") == 0) return RHASH_MD5;
    if (strcasecmp(name, "sha1") == 0) return RHASH_SHA1;
    if (strcasecmp(name, "tth") == 0) return RHASH_TTH;
    return 0;
}

int main() {
    rhash_library_init();
    unsigned char digest[64];
    char output[130];
    char *line = NULL;
    size_t len = 0;

    while (1) {
        // 1. 读取输入
#ifdef HAVE_LIBREADLINE
        char *input = readline(NULL);
        if (!input) break; // EOF (Ctrl+D)
        if (*input) add_history(input);
        line = input;
#else
        // 使用 getline
        ssize_t read_len = getline(&line, &len, stdin);
        if (read_len == -1) break; // EOF
        // 去除末尾换行符，以免干扰打印或逻辑
        if (line[read_len - 1] == '\n') line[read_len - 1] = '\0';
#endif

        // 2. 解析命令
        // 使用 strtok 分割空格
        char *cmd = strtok(line, " \t\r\n");
        if (!cmd) {
#ifdef HAVE_LIBREADLINE
            free(line);
#endif
            continue; // 空行
        }

        char *arg = strtok(NULL, " \t\r\n");
        if (!arg) {
            fprintf(stderr, "Error: missing argument\n");
#ifdef HAVE_LIBREADLINE
            free(line);
#endif
            continue;
        }

        // 3. 识别算法
        unsigned hash_id = get_hash_id(cmd);
        if (hash_id == 0) {
            fprintf(stderr, "Error: unknown hash algorithm '%s'\n", cmd);
#ifdef HAVE_LIBREADLINE
            free(line);
#endif
            continue;
        }

        // 4. 确定输出格式：大写->Hex, 小写->Base64
        int format = isupper(cmd[0]) ? RHPR_HEX : RHPR_BASE64;

        int res = 0;
        // 5. 处理输入源 (字符串 vs 文件)
        if (arg[0] == '"') {
            // 字符串模式：跳过第一个引号，计算剩余部分
            // 题目假设：不含空格，所以 strtok 已经切好了，我们只需处理 arg+1
            const char *str_data = arg + 1;
            res = rhash_msg(hash_id, str_data, strlen(str_data), digest);
            if (res < 0) {
                fprintf(stderr, "Error: hashing failed\n");
            } else {
                rhash_print_bytes(output, digest, rhash_get_digest_size(hash_id), format);
                printf("%s\n", output);
            }
        } else {
            // 文件模式
            res = rhash_file(hash_id, arg, digest);
            if (res < 0) {
                fprintf(stderr, "Error: file '%s' - %s\n", arg, strerror(errno));
            } else {
                rhash_print_bytes(output, digest, rhash_get_digest_size(hash_id), format);
                printf("%s\n", output);
            }
        }

#ifdef HAVE_LIBREADLINE
        free(line);
#endif
    }

#ifndef HAVE_LIBREADLINE
    free(line);
#endif
    return 0;
}

