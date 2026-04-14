include ak/ak.cfg.mk

CFLAGS += -I./ak/inc
CPPFLAGS += -I./ak/inc

VPATH += ak/src

SOURCES += ak/src/fsm.c
SOURCES += ak/src/tsm.c
SOURCES += ak/src/task.c
SOURCES += ak/src/timer.c
SOURCES += ak/src/message.c
