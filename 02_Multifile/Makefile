# 编译器与工具
CC       := gcc
CFLAGS   := -fPIC -Wall -O2
AR       := ar
ARFLAGS  := rcs
RM       := rm -f

# 源文件与对象文件
SRCS     := fun.c const.c prog.c
OBJS     := $(SRCS:.c=.o)
LIB_OBJS := fun.o const.o

# 库文件
STATIC_LIB := liboutput_static.a
SHARED_LIB := liboutput.so

# 可执行文件
PROGS    := prog prog-a prog-so

.PHONY: all test clean

# 默认目标：生成静态库、共享库及所有可执行文件
all: $(STATIC_LIB) $(SHARED_LIB) $(PROGS)

# 生成静态库
$(STATIC_LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

# 生成共享库
$(SHARED_LIB): $(LIB_OBJS)
	$(CC) -shared -o $@ $^

# 通用 .o 编译规则
%.o: %.c outlib.h
	$(CC) $(CFLAGS) -c $< -o $@

# prog：链接静态库
prog: prog.o $(STATIC_LIB)
	$(CC) -o $@ prog.o $(STATIC_LIB)

# prog-a：同样链接静态库
prog-a: prog.o $(STATIC_LIB)
	$(CC) -o $@ prog.o $(STATIC_LIB)

# prog-so：链接共享库并设置运行时库路径
prog-so: prog.o $(SHARED_LIB)
	$(CC) -o $@ prog.o -L. -l:$(SHARED_LIB) -Wl,-rpath=.

# 测试目标：无参、单参、三参运行并比较输出
test: all
	@echo "=== 无参测试 ==="
	@for p in $(PROGS); do \
	  ./$$p >$$p.noarg.out 2>&1; \
	done
	@echo "=== 单参数测试 ==="
	@for p in $(PROGS); do \
	  ./$$p foo >$$p.one.out 2>&1; \
	done
	@echo "=== 三参数测试 ==="
	@for p in $(PROGS); do \
	  ./$$p a b c >$$p.three.out 2>&1; \
	done
	@echo "=== 比较输出 ==="
	@for p in $(PROGS); do \
	  cmp $$p.noarg.out $$p.one.out >/dev/null && echo "$$p: noarg vs one OK" || echo "$$p: noarg vs one MISMATCH"; \
	  cmp $$p.noarg.out $$p.three.out >/dev/null && echo "$$p: noarg vs three OK" || echo "$$p: noarg vs three MISMATCH"; \
	done

# 清理所有生成文件
clean:
	$(RM) $(OBJS) $(STATIC_LIB) $(SHARED_LIB) $(PROGS) \
	       *.noarg.out *.one.out *.three.out

