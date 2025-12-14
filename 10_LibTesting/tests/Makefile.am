TESTS = dummy upstream
check_PROGRAMS = dummy upstream

.ts.c:
	checkmk $< > $@

AM_CFLAGS = -I$(top_builddir)/src @CHECK_CFLAGS@
LDADD = $(top_builddir)/src/libnamegen.la @CHECK_LIBS@
