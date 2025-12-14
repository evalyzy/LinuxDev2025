#!/bin/bash

echo "Running tests..."

# 临时文件
OUT_FILE="test_output.txt"

# 辅助函数：标准化输出（去除所有空白字符）进行比较
normalize() {
    echo "$1" | tr -d '[:space:]'
}

# --- 测试 1: MD5 字符串 (Hex 格式) ---
# 注意：echo 默认会输出换行符。'echo "hello"' 实际上是 "hello\n"。
# 如果你的 rhasher 是按行读取 getline，它会把换行符去掉。
# 但为了确保 md5sum 计算的一致性，我们需要明确 echo 是否带换行。
# 你的 C 代码逻辑是：去除末尾 \n，然后计算 rhash_msg(str_data)。
# 比如输入 "MD5 "hello\n"，你的代码会解析出 arg="hello"，然后计算 md5("hello")。
# 而 `echo "hello" | md5sum` 计算的是 "hello\n" 的 MD5！
# 必须使用 `echo -n "hello"` 来让 md5sum 计算 "hello" 的 MD5。

INPUT_STR="hello"
CMD="MD5 \"$INPUT_STR"
# 运行 rhasher
echo "$CMD" | ./rhasher > "$OUT_FILE" 2>&1

# 计算期望值 (注意使用 -n)
EXPECTED=$(echo -n "$INPUT_STR" | md5sum | awk '{print $1}')
# 获取实际值 (只取最后一行，防止有其他调试信息)
ACTUAL=$(tail -n 1 "$OUT_FILE")

# 比较
if [ "$(normalize "$ACTUAL")" == "$(normalize "$EXPECTED")" ]; then
    echo "[PASS] MD5 String Hex"
else
    echo "[FAIL] MD5 String Hex"
    echo "  Input:    $CMD"
    echo "  Expected: $EXPECTED"
    echo "  Got:      $ACTUAL"
    exit 1
fi

# --- 测试 2: SHA1 字符串 (Hex 格式) ---
INPUT_STR="world"
CMD="SHA1 \"$INPUT_STR"

echo "$CMD" | ./rhasher > "$OUT_FILE" 2>&1
EXPECTED=$(echo -n "$INPUT_STR" | sha1sum | awk '{print $1}')
ACTUAL=$(tail -n 1 "$OUT_FILE")

if [ "$(normalize "$ACTUAL")" == "$(normalize "$EXPECTED")" ]; then
    echo "[PASS] SHA1 String Hex"
else
    echo "[FAIL] SHA1 String Hex"
    echo "  Input:    $CMD"
    echo "  Expected: $EXPECTED"
    echo "  Got:      $ACTUAL"
    exit 1
fi

# 清理
rm -f "$OUT_FILE"
echo "All tests passed!"

