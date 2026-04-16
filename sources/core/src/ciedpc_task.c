/**
 * @file ciedpc_task.c
 * @author Shang Huang
 * @brief Implementation of task management for CIEDPC system
 * @version 0.1
 * @date 2026-04-16
 * 
 * @copyright MIT License
 * 
 */
#include <string.h>
#include "ciedpc_core.h"
#include "ciedpc_task.h"
#include "ciedpc_msg.h"
#include "fifo.h"

/**
 * @brief Khai báo các biến toàn cục quản lý thông tin của các tác vụ trong hệ thống CIEDPC
 */

CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta task_norm_t g_current_task; // Cấu trúc quản lý thông tin của tác vụ hiện tại đang được thực thi, bao gồm ID, mức độ ưu tiên, hàm thực thi và hàng đợi tin nhắn của tác vụ đó
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta task_id_t g_active_task_id = CIEDPC_TASK_IDLE_ID; // ID của tác vụ hiện tại đang được thực thi, được khởi tạo với giá trị của tác vụ trống (idle) để biểu thị rằng hệ thống đang ở trạng thái nhàn rỗi
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta ciedpc_msg_t* g_current_msg; // Con trỏ đến tin nhắn hiện tại đang được xử lý bởi tác vụ hiện tại
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta task_pri_t g_task_ready; // Biến bitmap quản lý trạng thái sẵn sàng của các tác vụ, mỗi bit tương ứng với một mức độ ưu tiên của tác vụ
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta task_norm_t* g_task_norm_table = NULL;
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta task_polling_t* g_task_polling_table = NULL;
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta ui8 g_task_norm_count = 0;
CIEDPC_ATTR_SECTION(".ciedpc_task_section") sta ui8 g_task_polling_count = 0;

sta void internal_ciedpc_task_put_to_queue(task_id_t tid, ciedpc_msg_t* msg);
sta ciedpc_msg_t* internal_ciedpc_task_get_from_queue(task_id_t tid);
sta void internal_ciedpc_task_set_ready(task_pri_t pri);
sta void internal_ciedpc_task_clear_ready(task_pri_t pri);
sta task_pri_t internal_ciedpc_task_find_highest_priority(void);
sta void internal_ciedpc_task_dispatch(task_norm_t* task, ciedpc_msg_t* msg);
sta void internal_ciedpc_task_polling_exec(void);
sta task_norm_t* internal_get_task_by_id(task_id_t tid);

void ciedpc_task_norm_create(task_norm_t* task_table) {

}

void ciedpc_task_polling_create(task_polling_t* task_table) {

}

RETR_STAT ciedpc_task_post_msg(task_id_t dest_id, ciedpc_msg_t* msg) {

}

RETR_STAT ciedpc_task_post_isr(task_id_t dest_id, ui8 sig) {

}

RETR_STAT ciedpc_task_scheduler() {

}

task_id_t ciedpc_task_get_current_id() {

}

ciedpc_msg_t* ciedpc_task_get_current_msg() {

}

bool ciedpc_task_is_ready(task_id_t task_id) {

}

/**
 * @brief Hàm nội bộ để đưa một tin nhắn vào hàng đợi của một tác vụ cụ thể
 * 
 * @param tid ID của tác vụ mà tin nhắn sẽ được đưa vào hàng đợi
 * @param msg Con trỏ đến tin nhắn cần đưa vào hàng đợi
 */
void internal_ciedpc_task_put_to_queue(task_id_t tid, ciedpc_msg_t* msg) {

}

/**
 * @brief Hàm nội bộ để lấy một tin nhắn từ hàng đợi của một tác vụ cụ thể
 * 
 * @param tid ID của tác vụ mà tin nhắn sẽ được lấy từ hàng đợi
 * @return ciedpc_msg_t* Con trỏ đến tin nhắn được lấy từ hàng đợi, hoặc NULL nếu hàng đợi trống
 */
ciedpc_msg_t* internal_ciedpc_task_get_from_queue(task_id_t tid) {

}

/**
 * @brief Hàm nội bộ để thiết lập trạng thái sẵn sàng cho một tác vụ dựa trên mức độ ưu tiên của nó
 * 
 * @param pri Mức độ ưu tiên của tác vụ cần thiết lập trạng thái sẵn sàng
 */
void internal_ciedpc_task_set_ready(task_pri_t pri) {
	
}

/**
 * @brief Hàm nội bộ để xóa trạng thái sẵn sàng của một tác vụ dựa trên mức độ ưu tiên của nó
 * 
 * @param pri Mức độ ưu tiên của tác vụ cần xóa trạng thái sẵn sàng
 */
void internal_ciedpc_task_clear_ready(task_pri_t pri) {

}

/**
 * @brief Hàm nội bộ để tìm mức độ ưu tiên cao nhất của các tác vụ đang ở trạng thái sẵn sàng
 * 
 * @return task_pri_t Mức độ ưu tiên cao nhất của các tác vụ đang ở trạng thái sẵn sàng, hoặc 0 nếu không có tác vụ nào sẵn sàng
 */
task_pri_t internal_ciedpc_task_find_highest_priority(void) {
	
}

/**
 * @brief Hàm nội bộ để điều phối việc thực thi của một tác vụ cụ thể với một tin nhắn cụ thể
 * 
 * @param task Con trỏ đến cấu trúc thông tin của tác vụ cần điều phối
 * @param msg Con trỏ đến tin nhắn cần được xử lý bởi tác vụ
 */
void internal_ciedpc_task_dispatch(task_norm_t* task, ciedpc_msg_t* msg) {
	
}

/**
 * @brief Hàm nội bộ để thực thi các tác vụ polling có khả năng được bật
 */
void internal_ciedpc_task_polling_exec(void) {
	
}

/**
 * @brief Hàm nội bộ để tìm một tác vụ trong bảng tác vụ dựa trên ID của nó
 * 
 * @param tid ID của tác vụ cần tìm kiếm
 * @return task_norm_t* Con trỏ đến cấu trúc thông tin của tác vụ nếu tìm thấy, hoặc NULL nếu không tìm thấy
 */
task_norm_t* internal_get_task_by_id(task_id_t tid) {

}