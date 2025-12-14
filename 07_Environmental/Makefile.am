bin_PROGRAMS = rhasher
rhasher_SOURCES = rhasher.c
# 链接 rhash 和可能的 readline
rhasher_LDADD = -lrhash $(READLINE_LIBS)

# 测试脚本
TESTS = tests.sh

# 清理规则 (git clean 是一种激进的方法，但在作业中很常用)
gitclean:
	git clean -dfX

