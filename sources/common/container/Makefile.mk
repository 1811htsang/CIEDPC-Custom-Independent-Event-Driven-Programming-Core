CFLAGS		+= -I./common/container
CPPFLAGS	+= -I./common/container

VPATH += common/container

SOURCES += common/container/log_queue.c
SOURCES += common/container/fifo.c
SOURCES += common/container/ring_buffer.c
