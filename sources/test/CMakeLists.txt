# Khai báo các tệp sử dụng trong dự án
# core/inc/*
# core/src/*
# common/container/fifo/*
# test/*
# pal/pal_core.h

cmake_minimum_required(VERSION 3.10)

project(test C)

# Định nghĩa các file nguồn .c sử dụng trong dự án
set(SOURCES
  test.c
  ../core/src/ciedpc_fsm.c
  ../core/src/ciedpc_tsm.c
  ../core/src/ciedpc_msg.c
  ../core/src/ciedpc_task.c
  ../core/src/ciedpc_timer.c

  ../common/container/fifo/fifo.c
)

# Cấu hình để luôn tạo ra file Debug (tương đương flag -g)
set(CMAKE_BUILD_TYPE Debug)

# Tạo mục tiêu thực thi
add_executable(${PROJECT_NAME} ${SOURCES})

# Định nghĩa các Macro (tương đương flag -D)

# Cấu hình các đường dẫn Include (tương đương các tham số -I)
target_include_directories(${PROJECT_NAME} PRIVATE
  ../core/inc/
  ../core/src/
  ../common/container/fifo/
  ../pal/
  ./
)


