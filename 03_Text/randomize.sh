#!/bin/sh
# POSIX / dash-compatible ASCII-art random painter (Simple Fixed Version)
# Usage: ./randomize.sh [delay_seconds]

set -eu

DELAY="${1:-0}"
export LC_ALL=C
TMP_COORDS=""

cleanup() {
    [ -n "$TMP_COORDS" ] && rm -f "$TMP_COORDS" 2>/dev/null
    if terminal_info="$(stty size </dev/tty 2>/dev/null)"; then
        set -- $terminal_info
        rows="$1"
        if [ "$rows" -gt 0 ] 2>/dev/null; then
            tput cup "$((rows - 1))" 0 2>/dev/null || true
        fi
    fi
}

trap cleanup EXIT INT TERM HUP

# Validate delay
case "$DELAY" in
    ''|*[!0-9.]*|.*.*.*) 
        if [ "$DELAY" != "0" ]; then
            echo "Error: Invalid delay '$DELAY'" >&2
            exit 1
        fi
        ;;
esac

tput clear 2>/dev/null || true
TMP_COORDS="$(mktemp)"

# Process input and create coordinates file
{
    x=0
    y=0
    od -An -t u1 -v -w1 | while IFS= read -r b; do
        [ -n "$b" ] || continue
        b="$(printf '%s' "$b" | tr -d ' \t')"
        [ -n "$b" ] || continue
        
        if [ "$b" -eq 10 ] 2>/dev/null; then
            y=$((y + 1))
            x=0
            continue
        fi
        
        if [ "$b" -eq 13 ] 2>/dev/null; then
            continue
        fi
        
        if [ "$b" -ge 32 ] && [ "$b" -le 126 ] 2>/dev/null; then
            printf '%d %d %d\n' "$y" "$x" "$b"
        fi
        x=$((x + 1))
    done
} >"$TMP_COORDS"

# Check if coordinates file has content
if [ ! -s "$TMP_COORDS" ]; then
    echo "No valid characters found" >&2
    exit 0
fi

# Display characters randomly
shuf "$TMP_COORDS" | while IFS=' ' read -r Y X B; do
    tput cup "$Y" "$X" 2>/dev/null || continue
    # 1) 先把数字转换成三位八进制字符串
    octal=$(printf '%03o' "$B")
    # 2) 用 %b 格式让 printf 展开转义，从而真正输出该字节
    printf '%b' "\\$octal"
    if [ "$DELAY" != "0" ]; then
        sleep "$DELAY"
    fi
done

printf '\n'
