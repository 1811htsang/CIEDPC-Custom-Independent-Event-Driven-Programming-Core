// Khai báo các thư viện sử dụng
#include "ak.h"
#include "sys_cfg.h"
#include "sys_ctrl.h"
#include "ak.h"
#include "sys_dbg.h"

// Khai báo biến toàn cục để lưu trữ thông tin fatal log
static fatal_log_t fatal_log;

void sys_dbg_fatal(const int8_t* s, uint8_t c) {
	// Khai báo ủy quyền cho linker script để lấy địa chỉ của vùng RAM và stack
	extern uint32_t _start_ram;
	extern uint32_t _estack;

	// Khai báo con trỏ tạm thời để lưu trữ thông tin về tác vụ hiện tại và đối tượng tin nhắn hiện tại
	task_t*		ptemp_current_task;
	ak_msg_t*	ptemp_current_active_object;

	// Lưu thông tin fatal log vào vùng RAM để có thể truy xuất sau khi khởi động lại hệ thống
	memset(fatal_log.string, 0, 10);
	strcpy((char*)fatal_log.string, (const char*)s);

	// Lưu mã lỗi vào fatal log
	fatal_log.code = c;

	// Lấy thông tin về tác vụ hiện tại và đối tượng tin nhắn hiện tại để lưu vào fatal log
	ptemp_current_task 			= get_current_task_info();
	ptemp_current_task->id 	= get_current_task_id();

	// Lấy thông tin về đối tượng tin nhắn hiện tại đang được xử lý bởi tác vụ hiện tại để lưu vào fatal log
	ptemp_current_active_object = get_current_active_object();

	// Lưu thông tin về các thanh ghi của core M3 vào fatal log để có thể phân tích
	fatal_log.m3_core_reg.ipsr			= __get_IPSR();
	fatal_log.m3_core_reg.primask		= __get_PRIMASK();
	fatal_log.m3_core_reg.faultmask	= __get_FAULTMASK();
	fatal_log.m3_core_reg.basepri		= __get_BASEPRI();
	fatal_log.m3_core_reg.control		= __get_CONTROL();

	// Lưu thông tin về số lần khởi động lại hệ thống vào fatal log để có thể theo dõi
	memcpy(&fatal_log.current_task, ptemp_current_task, sizeof(task_t));
	memcpy(&fatal_log.current_active_object, ptemp_current_active_object, sizeof(ak_msg_t));

	// Lưu thông tin fatal log vào flash để có thể truy xuất sau khi khởi động lại hệ thống
	sys_ctrl_delay_us(1000);

	while(1) {
		// Cho vào loop để tránh lỗi
	}
}

