CFLAGS  = -ansi -pedantic -Wall -Wextra -O3 -g3

lib_LTLIBRARIES = libnamegen.la
bin_PROGRAMS = namegen

namegen_LDADD = libnamegen.la

libnamegen.c: const.h

const.h: tablegen.py
	python3 $< > $@

check: namegen
	@./namegen --test
