/**
 * @file pal_memrp.c
 * @author Shang Huang
 * @brief Implementation of PAL Memory Reporter for CIEDPC project
 * @version 0.1
 * @date 2026-04-30
 * @copyright MIT License
 */

#include <string.h>
#include "ciedpc_core.h"
#include "pal_memrp.h"
#include "ciedpc_task.h"
#include "ciedpc_msg.h"
#include "fifo.h"

void pal_memrp_report(pal_memrp_info_t* info) {
  if (info == NULL || info->target == NULL) {
    return; // Nếu thông tin không hợp lệ, không thực hiện báo cáo
  }

  // Tùy thuộc vào loại target, chúng ta sẽ lấy thông tin bộ nhớ khác nhau
  if (info->target == timer_ctrl.free_list || info->target == timer_ctrl.head) {
    // Báo cáo thông tin bộ nhớ của Timer Pool
    info->used = timer_ctrl.active_count;
    info->max_used = CIEDPC_TIMER_MAX_NODES - 1; // Giả sử max_used là số lượng nút đã từng được sử dụng trong Pool
    info->total = CIEDPC_TIMER_MAX_NODES;
  } else {
    // Có thể thêm các loại target khác như Task Queue, Alloc Pool, v.v. ở đây
    // Ví dụ: nếu target là một hàng đợi tin nhắn của một tác vụ cụ thể, chúng ta có thể lấy thông tin từ đó
  }
}