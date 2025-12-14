#!/bin/bash

echo "==================== STARTING TESTS ===================="

# 1. 正常移动测试
echo "[TEST 1] Normal move"
echo "Hello World" > file_source
./move file_source file_dest
if [ ! -f file_source ] && [ -f file_dest ]; then
    echo "PASS: Moved successfully"
else
    echo "FAIL: Normal move failed"
fi
rm -f file_dest

# 2. LD_PRELOAD 保护测试
echo "[TEST 2] LD_PRELOAD protection"
echo "Important Data" > PROTECT_me
LD_PRELOAD=./protect.so ./move PROTECT_me output_new 2>/dev/null
if [ -f PROTECT_me ] && [ -f output_new ]; then
    echo "PASS: Source file preserved (Protected)"
else
    echo "FAIL: Protection failed"
fi
rm -f output_new PROTECT_me

# 3. Strace Error Injection - Open Input Failure
# 之前的测试这个是 PASS 的，保持原样 (-P file_in 有效)
echo "[TEST 3] Strace: Inject open error (ENOENT)"
echo "Data" > file_in
strace -P file_in -e inject=openat:error=ENOENT ./move file_in file_out 2> strace_log
if grep -q "No such file or directory" strace_log && [ -f file_in ]; then
    echo "PASS: Handled input open error"
else
    echo "FAIL: Did not handle input open error correctly"
fi
rm -f file_in strace_log

# 4. Strace Error Injection - Write Failure
echo "[TEST 4] Strace: Inject write error (ENOSPC)"
echo "Some Data to write" > file_in
# 修正：去掉 -P 参数，改用 when=1。
# 因为程序启动通常不调用 write，这是用户代码的第一次 write，捕捉率 100%
strace -e inject=write:error=ENOSPC:when=1 ./move file_in file_out 2> strace_log
if [ -f file_in ] && [ ! -f file_out ]; then
    echo "PASS: Cleaned up output file after write error"
else
    echo "FAIL: Cleanup failed or source deleted"
    # 调试信息：如果失败，显示 strace 里的 write 调用情况
    grep "write" strace_log | head -n 5
fi
rm -f file_in file_out strace_log

# 5. Strace Error Injection - Close Output Failure
echo "[TEST 5] Strace: Inject close output error (EIO)"
echo "Data" > file_in
# 修正技巧：先 touch file_out，确保文件存在，帮助 strace -P 解析路径
touch file_out
# 依然使用 -P file_out，但因为文件已存在，inode 追踪会更可靠
strace -P file_out -e inject=close:error=EIO ./move file_in file_out 2> strace_log
if grep -q "Error closing output file" strace_log && [ -f file_in ]; then
    echo "PASS: Handled close error and cleaned up"
else
    echo "FAIL: Did not handle close error correctly"
    # 调试信息
    grep "close" strace_log
fi
rm -f file_in file_out strace_log

echo "==================== TESTS COMPLETED ===================="

