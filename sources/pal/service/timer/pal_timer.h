/**
 * @file pal_timer.h
 * @author Shang Huang
 * @brief PAL Timer header file for CIEDPC project
 * @version 0.1
 * @date 2026-04-30
 * 
 * @copyright MIT License
 * 
 */
#ifndef __PAL_TIMER_H__
  #define __PAL_TIMER_H__

  /**
   * @brief Khai báo thư viện sử dụng
   */

  #include <ciedpc_core.h>

  /**
   * @brief Biến toàn cục lưu trữ số tick của hệ thống, được cập nhật bởi hàm pal_timer_tick() mỗi khi có tick mới
   */
  sta vol ui32 system_tick = 0; 

  /**
   * @brief Hàm khởi tạo timer, cần được gọi trước khi sử dụng bất kỳ chức năng nào liên quan đến timer
   */
  CIEDPC_ATTR_WEAK void pal_timer_init(void);

  /**
   * @brief Hàm tick của timer, cần được gọi định kỳ (ví dụ: mỗi 1ms) để cập nhật trạng thái của timer 
   *        và kích hoạt các tác vụ đã hết thời gian chờ
   */
  CIEDPC_ATTR_WEAK void pal_timer_tick(void);

  /**
   * @brief Hàm lấy số tick hiện tại của hệ thống, có thể được sử dụng để đo thời gian hoặc lập lịch tác vụ
   * @return Số tick hiện tại của hệ thống
   */
  stinl ui32 pal_timer_get_tick(void) {
    return system_tick;
  }

#endif // __PAL_TIMER_H__