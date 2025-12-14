AC_PREREQ([2.69])
AC_INIT([solver], [1.0], [student@example.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# 检查编译器
AC_PROG_CC

# 国际化支持 (gettext)
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 po/Makefile.in
                 man/Makefile
                 tests/Makefile])
AC_OUTPUT
