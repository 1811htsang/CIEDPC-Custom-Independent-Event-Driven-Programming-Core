CFLAGS		+= -I./app
CPPFLAGS	+= -I./app

VPATH += app

# CPP source files
SOURCES_CPP += app/task_system.cpp
SOURCES_CPP += app/app.cpp
SOURCES_CPP += app/app_data.cpp
SOURCES_CPP += app/app_non_clear_ram.cpp

SOURCES_CPP += app/task_life.cpp
SOURCES_CPP += app/task_list.cpp

SOURCES_CPP += app/task_if.cpp
SOURCES_CPP += app/task_uart_if.cpp
SOURCES_CPP += app/task_dbg.cpp
