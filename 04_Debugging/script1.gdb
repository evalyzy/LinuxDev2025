# script1.gdb
set pagination off

break should_print if current % 5 == 0
commands 1
  printf "start=%d stop=%d step=%d curval=%d\n", start, stop, step, current, idx
  cont
end

run 1 12 > /dev/null
quit

