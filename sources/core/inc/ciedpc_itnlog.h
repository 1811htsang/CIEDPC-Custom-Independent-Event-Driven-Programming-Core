/**
 * @file ciedpc_itnlog.h
 * @author Shang Huang
 * @brief Internal logger for CIEDPC
 * @version 0.1
 * @date 2026-05-14
 * @copyright MIT License
 */
#ifndef CIEDPC_ITNLOG_H
  #define CIEDPC_ITNLOG_H

  /**
   * @brief Khai báo thư viện sử dụng
   */
  #include "ciedpc_core.h"
  #include "ciedpc_fsm.h"
  #include "ciedpc_tsm.h"
  #include <stdint.h>
  
  /**
   * @brief Định nghĩa các mức độ log
   */
  typedef enum {
    ITNLOG_LEVEL_DEBUG = 0,
    ITNLOG_LEVEL_INFO,
    ITNLOG_LEVEL_WARN,
    ITNLOG_LEVEL_ERROR,
    ITNLOG_LEVEL_FATAL
  } itnlog_level_t;

  /**
   * @brief Định nghĩa các thẻ log mặc định cho các module của CIEDPC
   */
  #define ITNLOG_TAG_TSK  "TSK"
  #define ITNLOG_TAG_MSG  "MSG"
  #define ITNLOG_TAG_FSM  "FSM"
  #define ITNLOG_TAG_TSM  "TSM"
  #define ITNLOG_TAG_TIM  "TIM"

  /**
   * @brief Định nghĩa cấu trúc log
   * @param level Mức độ log
   * @param tag Thẻ log để phân loại log theo module
   * @param task_id ID của tác vụ liên quan đến log (nếu có)
   * @param msg_sig Sig của tin nhắn liên quan đến log (nếu có)
   * @param fsm Con trỏ đến FSM liên quan đến log (nếu có)
   * @param tsm Con trỏ đến TSM liên quan đến log (nếu có)
   * @param message Nội dung log
   * @param timestamp Thời gian log (đơn vị: ms)
   * @param checksum Giá trị checksum của log entry, 
   *        được tính toán dựa trên nội dung của log entry 
   *        để đảm bảo tính toàn vẹn của dữ liệu log.
   *        Tránh việc ghi đè hoặc mất dữ liệu log 
   *        do lỗi ghi log hoặc lỗi bộ nhớ.
   * @attention `fsm` và `tsm` là con trỏ đến các cấu trúc FSM và TSM tương ứng, 
   *            cho phép log ghi lại thông tin chi tiết về trạng thái 
   *            và chuyển đổi trạng thái của FSM và TSM liên quan đến log đó.
   *            Nếu không có sử dụng `fsm` hoặc `tsm`, có thể để con trỏ này là NULL 
   *            để biểu thị không có thông tin FSM hoặc TSM liên quan đến log đó.
   */
  typedef struct {
    itnlog_level_t level;
    const char* tag;
    ui16 task_id;
    ui16 msg_sig;
    ciedpc_fsm_t* fsm;
    ciedpc_tsm_t* tsm;
    const char* message;
    ui32 timestamp;
    ui16 checksum; 
  } itnlog_entry_t;


#endif // CIEDPC_ITNLOG_H