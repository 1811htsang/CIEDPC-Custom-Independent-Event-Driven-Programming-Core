#ifndef __SYS_CFG_H__
  #define __SYS_CFG_H__

  // Khai báo directive C cho C++
  #ifdef __cplusplus
  extern "C"
  {
  #endif

    // Khai báo thư viện sử dụng
    #include <stdint.h>
    #include "main.h"
    #include "system.h"

    // Khai báo các hàm cấu hình hệ thống
    void sys_cfg_init(void);        // Hàm khởi tạo cấu hình hệ thống
    void sys_cfg_clock(void);       // Hàm cấu hình hệ thống clock
    void sys_cfg_tick(void);        // Hàm cấu hình hệ thống tick (systick)
    void sys_cfg_update_info(void); // Hàm cập nhật thông tin hệ thống 

    /**
     * Ghi chú:
     * Phần này đang trong quá trình cân nhắc
     * loại bỏ do các chức năng có thể được HAL xử lý.
     */

  #ifdef __cplusplus
  }
  #endif

#endif
