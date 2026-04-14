include common/container/Makefile.mk
#include sources/common/ctreemap/Makefile.mk

CFLAGS		+= -I./common
CPPFLAGS	+= -I./common

VPATH += common

SOURCES += common/utils.c
SOURCES += common/xprintf.c
