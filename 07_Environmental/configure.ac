AC_PREREQ([2.69])
AC_INIT([rhasher], [1.0], [user@localhost])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([rhasher.c])
AC_CONFIG_HEADERS([config.h])

# 检查编译器
AC_PROG_CC

# 1. 检查 librhash (必需)
# 检查库函数 rhash_library_init 是否在库 rhash 中
AC_CHECK_LIB([rhash], [rhash_library_init], [], [AC_MSG_ERROR([librhash is required but not found])])
# 检查头文件
AC_CHECK_HEADERS([rhash.h], [], [AC_MSG_ERROR([rhash.h not found])])

# 2. 检查 libreadline (可选)
AC_ARG_ENABLE([readline],
    AS_HELP_STRING([--disable-readline], [Disable readline support]),
    [with_readline=$enableval],
    [with_readline=check])

# 如果用户没有显式禁用 (no)
AS_IF([test "x$with_readline" != "xno"],
    [AC_CHECK_LIB([readline], [readline],
        [
            # 如果找到，定义宏 HAVE_LIBREADLINE 并设置链接变量
            AC_DEFINE([HAVE_LIBREADLINE], [1], [Define if you have libreadline])
            READLINE_LIBS="-lreadline"
        ],
        [
            # 如果没找到且用户显式要求开启，则报错
            if test "x$with_readline" != "xcheck"; then
                AC_MSG_FAILURE([--enable-readline was given, but test failed])
            fi
        ]
    )]
)

# 导出变量给 Makefile 使用
AC_SUBST([READLINE_LIBS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

