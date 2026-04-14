#ifndef __SYS_CTRL_H__
  #define __SYS_CTRL_H__

  // Khai báo directive C cho C++
  #ifdef __cplusplus
  extern "C"
  {
  #endif

    // Khai báo thư viện sử dụng
    #include <stdint.h>
    #include "system.h"
    #include "platform.h"

    // Khai báo hàm reset hệ thống, sử dụng NVIC_SystemReset để khởi động lại vi điều khiển
    extern void sys_ctrl_reset();

    // Khai báo hàm quản lý watchdog
    extern void sys_ctrl_independent_watchdog_init();   // Khởi tạo watchdog độc lập
    extern void sys_ctrl_independent_watchdog_reset();  // Reset watchdog độc lập để ngăn chặn reset hệ thống

    // Khai báo hàm quản lý soft watchdog
    extern void sys_ctrl_soft_watchdog_init(uint32_t);      // Khởi tạo soft watchdog với thời gian timeout được chỉ định
    extern void sys_ctrl_soft_watchdog_reset();             // Reset soft watchdog, đặt lại bộ đếm tick về 0
    extern void sys_ctrl_soft_watchdog_enable();            // Kích hoạt soft watchdog, cho phép soft watchdog hoạt động
    extern void sys_ctrl_soft_watchdog_disable();           // Vô hiệu hóa soft watchdog, ngăn chặn soft watchdog hoạt động
    extern void sys_ctrl_soft_watchdog_increase_counter();  // Tăng bộ đếm của soft watchdog

    // Khai báo hàm tạo độ trễ, sử dụng vòng lặp đếm để tạo độ trễ chính xác
    extern void sys_ctrl_delay(volatile uint32_t count);

    // Khai báo hàm tạo độ trễ theo miligiây, sử dụng timer delay
    extern void sys_ctrl_delay_ms(volatile uint32_t count);

    // Khai báo hàm tạo độ trễ theo micro giây, sử dụng CPU delay dựa trên bộ đếm chu kỳ DWT để tạo độ trễ chính xác
    extern void sys_ctrl_delay_us(volatile uint32_t count);

    // Khai báo hàm để lấy biến timer hệ thống hiện tại theo đơn vị 1ns
    extern uint32_t sys_ctrl_micros();

    // Khai báo hàm để lấy biến timer hệ thống hiện tại theo đơn vị 1ms
    extern uint32_t sys_ctrl_millis();

    // Khai báo hàm để chuyển hệ thống vào chế độ ngủ và chờ ngắt
    extern void sys_ctrl_sleep_wait_for_irq();

    // Khai báo hàm để lấy số exception
    extern uint32_t sys_ctrl_get_exception_number();

    // Khai báo hàm để khởi động lại ứng dụng
    extern void sys_ctrl_restart_app();

    // Khai báo hàm để dừng MCU, đưa vi điều khiển vào chế độ standby để giảm thiểu tiêu thụ năng lượng
    extern void sys_ctrl_stop_mcu();

    // Khai báo hàm để lấy thông tin firmware, điền vào cấu trúc firmware_header_t với các giá trị như psk, độ dài binary và checksum
    extern int asm_test_add(int, int);

    // Khai báo hàm để lấy thông tin stack
    extern uint32_t sys_stack_count_init(); // Hàm này trả về kích thước stack ban đầu
    extern uint32_t sys_stack_usage();      // Hàm này trả về lượng stack đã sử dụng
    extern uint32_t sys_stack_get_size();   // Hàm này trả về tổng kích thước stack

  #ifdef __cplusplus
  }
  #endif

#endif // __SYS_CTRL_H__

