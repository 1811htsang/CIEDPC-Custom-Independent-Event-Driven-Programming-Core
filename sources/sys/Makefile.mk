CFLAGS	+= -I./sys
CPPFLAGS += -I./sys

VPATH += sys

SOURCES += sys/sys_dbg.c
SOURCES += sys/sys_boot.c
SOURCES_CPP += sys/porting_compat.cpp
