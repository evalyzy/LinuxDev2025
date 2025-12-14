AC_PREREQ([2.69])
AC_INIT([guess], [1.0], [user@localhost])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/guess.c])
AC_CONFIG_HEADERS([config.h])

# 检查编译器
AC_PROG_CC

# i18n 支持
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.21])

# 定义 LOCALEDIR 宏供 C 代码使用
localedir="\${datadir}/locale"
AC_SUBST(localedir)

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT

