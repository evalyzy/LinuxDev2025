# script2.gdb
set pagination off
set $COUNT = 0

break should_print if 28 <= ++$COUNT && $COUNT <= 35
commands 1
  printf "start=%d stop=%d step=%d curval=%d\n", start, stop, step, current, idx
  cont
end

run -100 100 3 > /dev/null
quit

