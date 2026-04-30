/**
 * @file pal_memrp.h
 * @author Shang Huang
 * @brief PAL Memory Reporter header file for CIEDPC project
 * @version 0.1
 * @date 2026-04-30
 * @copyright MIT License
 */
#ifndef __PAL_MEMRP_H__
  #define __PAL_MEMRP_H__

  /**
   * @brief Khai báo thư viện sử dụng
   */
  #include "ciedpc_core.h"
  #include "ciedpc_msg.h"

  /**
   * @brief Định nghĩa các loại đối tượng bộ nhớ trong hệ thống
   * @param MEMRP_TYPE_MSG_POOL Đối tượng là Pool quản lý tin nhắn của các tác vụ
   * @param MEMRP_TYPE_TASK_QUEUE Đối tượng là Hàng đợi của Task (FIFO)
   * @param MEMRP_TYPE_TIMER_POOL Đối tượng là Pool quản lý Timer
   * @param MEMRP_TYPE_ISR_FIFO Đối tượng là Hàng đợi tín hiệu ngắt
   * @param MEMRP_TYPE_UNKNOWN Loại đối tượng không xác định
   */
  typedef enum pal_memrp_type_t {
    MEMRP_TYPE_MSG_POOL = 0,
    MEMRP_TYPE_TASK_QUEUE,
    MEMRP_TYPE_TIMER_POOL,
    MEMRP_TYPE_ISR_FIFO,
    MEMRP_TYPE_UNKNOWN
  } pal_memrp_type_t;

  /**
   * @brief Cấu trúc thông tin báo cáo bộ nhớ cho một target cụ thể
   * @attention `target` có thể là bất kỳ loại Pool nào (Blank, Alloc, Extal, Timer, ISR, Task Queue, v.v.), 
   * @param target Con trỏ đến mảng chứa các elements như pool, task message queue, timer pool, 
   *               sử dụng void* để có thể áp dụng cho bất kỳ loại Pool nào (Blank, Alloc, Extal, Timer, ISR, Task Queue, v.v.)
   * @param used Số lượng elements đang được sử dụng trong Pool
   * @param max_used Số lượng elements tối đa đã từng được sử dụng trong Pool
   * @param total Tổng số elements có thể chứa trong Pool
   */
  typedef struct pal_memrp_info_t {
    void* target;
    ui8 used;
    ui8 max_used;
    ui8 total;
  } pal_memrp_info_t;

  /**
   * @brief Hàm báo cáo thông tin bộ nhớ của một target cụ thể
   * @param info Con trỏ đến cấu trúc chứa thông tin bộ nhớ cần báo cáo, bao gồm target, used, max_used và total
   */
  void pal_memrp_report(pal_memrp_info_t* info);

  /**
   * @brief Hàm báo cáo thông tin bộ nhớ của tất cả các target đã được định nghĩa trong hệ thống CIEDPC
   */
  void pal_memrp_report_all(void);

#endif