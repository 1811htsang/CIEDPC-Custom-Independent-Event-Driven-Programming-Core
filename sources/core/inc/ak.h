#ifndef __AK_H__
  #define __AK_H__

  // Khai báo C directive cho C++ 
  #ifdef __cplusplus
  extern "C"
  {
  #endif

    // Khai báo các thư viện sử dụng
    #include <string.h>
    #include "fsm.h"  // Thư viện FSM (Finite State Machine) để quản lý trạng thái của hệ thống
    #include "task.h" // Thư viện quản lý tác vụ, giúp tạo và quản lý các tác vụ trong hệ thống
    #include "port.h" // Thư viện quản lý attribute và dynamic allocation

    // Khai báo phiên bản của AK Firmware
    #define AK_VERSION						"1.0"

    // Khai báo các hằng số và macro để quản lý trạng thái, tín hiệu, và mức độ ưu tiên của tác vụ
    #define AK_ENABLE						(0x01u) // Quản lý trạng thái bật, giúp xác định khi nào một tính năng hoặc chức năng được kích hoạt trong hệ thống
    #define AK_DISABLE					(0x00u) // Quản lý trạng thái tắt, giúp xác định khi nào một tính năng hoặc chức năng được vô hiệu hóa trong hệ thống
    #define AK_FLAG_ON					(0x01u) // Quản lý trạng thái bật của cờ, giúp xác định khi nào một cờ hoặc điều kiện được kích hoạt trong hệ thống
    #define AK_FLAG_OFF					(0x00u) // Quản lý trạng thái tắt của cờ, giúp xác định khi nào một cờ hoặc điều kiện được vô hiệu hóa trong hệ thống
    #define AK_RET_OK						(0x01u) // Quản lý trạng thái trả về thành công, giúp xác định khi nào một hàm hoặc thao tác đã hoàn thành thành công trong hệ thống
    #define AK_RET_NG						(0x00u) // 

    // Khai báo hằng số cho tín hiệu tự định nghĩa của người dùng
    #define AK_USER_DEFINE_SIG				(10)

    // Khai báo hằng số cho mức độ ưu tiên của tác vụ
    #define TASK_PRI_MAX_SIZE				(8)
    #define TASK_PRI_LEVEL_0				(0) // Ưu tiên thấp nhất
    #define TASK_PRI_LEVEL_1				(1)
    #define TASK_PRI_LEVEL_2				(2)
    #define TASK_PRI_LEVEL_3				(3)
    #define TASK_PRI_LEVEL_4				(4)
    #define TASK_PRI_LEVEL_5				(5)
    #define TASK_PRI_LEVEL_6				(6)
    #define TASK_PRI_LEVEL_7				(7)

    // Khai báo hằng số cho ID của tác vụ
    #define AK_TASK_IDLE_ID					(0xEF) // Quản lý ID của tác vụ nhàn rỗi, giúp xác định và xử lý tác vụ nhàn rỗi trong hệ thống

  #ifdef __cplusplus
  }
  #endif

#endif // __AK_H__

