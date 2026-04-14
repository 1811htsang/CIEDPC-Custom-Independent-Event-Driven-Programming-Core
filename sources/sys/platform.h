#ifndef __PLATFORM_H__
  #define __PLATFORM_H__

  // Khai báo directive C cho C++
  #ifdef __cplusplus
  extern "C"
  {
  #endif

  // Khai báo thư viện sử dụng
  #include "stm32f1xx_hal.h"

  // Khai báo hàm enry/exit critical section
  void platform_enter_critical(void);
  void platform_exit_critical(void);

  // Định nghĩa các macro quản lý interrupt
  #define DISABLE_INTERRUPTS() __disable_irq()
  #define ENABLE_INTERRUPTS() __enable_irq()

  // Định nghĩa các macro quản lý critical section
  #define entry_critical() platform_enter_critical()
  #define exit_critical() platform_exit_critical()

  // Định nghĩa macro để lấy vị trí bit ưu tiên cao nhất đang sẵn sàng từ biến mask
  #ifndef LOG2LKUP
    /*
    * Trả về vị trí bit ưu tiên cao nhất đang sẵn sàng từ biến mask.
    * Lưu ý: x bắt buộc khác 0. Gọi __CLZ(0) sẽ tạo hành vi không xác định.
    */
    #define LOG2LKUP(x) ((uint32_t)(31U - __CLZ((uint32_t)(x))))
  #endif

  #ifdef __cplusplus
  }
  #endif

#endif