#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define BUF_SIZE 4096

// 辅助函数：打印错误并退出
void fatal_error(const char *msg, int exit_code) {
    perror(msg);
    exit(exit_code);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <infile> <outfile>\n", argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];
    
    int fd_in, fd_out;
    ssize_t bytes_read, bytes_written;
    char buffer[BUF_SIZE];

    // 1. 打开源文件 (只读)
    fd_in = open(infile, O_RDONLY);
    if (fd_in < 0) {
        fatal_error("Error opening input file", 2);
    }

    // 2. 打开目标文件 (写入, 创建, 截断)
    // 权限设为 0644
    fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        close(fd_in);
        fatal_error("Error opening output file", 3);
    }

    // 3. 复制数据
    while ((bytes_read = read(fd_in, buffer, BUF_SIZE)) > 0) {
        char *ptr = buffer;
        ssize_t to_write = bytes_read;
        
        while (to_write > 0) {
            bytes_written = write(fd_out, ptr, to_write);
            if (bytes_written < 0) {
                // 写入出错：尝试清理
                perror("Error writing to output file");
                close(fd_in);
                close(fd_out);
                unlink(outfile); // 删除不完整的目标文件
                return 4;
            }
            to_write -= bytes_written;
            ptr += bytes_written;
        }
    }

    // 检查 read 是否出错
    if (bytes_read < 0) {
        perror("Error reading input file");
        close(fd_in);
        close(fd_out);
        unlink(outfile); // 清理
        return 5;
    }

    // 4. 安全关闭
    // 关闭输出文件时也要检查错误（可能在这里发生磁盘满等错误）
    if (close(fd_out) < 0) {
        perror("Error closing output file");
        close(fd_in);
        unlink(outfile); // 清理
        return 6;
    }

    close(fd_in);

    // 5. 删除源文件
    if (unlink(infile) < 0) {
        perror("Error deleting input file (move completed but source remains)");
        return 7;
    }

    return 0;
}

