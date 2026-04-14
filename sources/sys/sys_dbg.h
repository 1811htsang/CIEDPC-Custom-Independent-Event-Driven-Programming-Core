#ifndef __SYS_DBG_H__
	#define __SYS_DBG_H__

	// Khai báo directive C cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>
		#include "task.h"
		#include "message.h"
		#include "sys_ctrl.h"

		// Định nghĩa cấu trúc để lưu trữ thông tin trên core M3 khi xảy ra lỗi fatal, bao gồm các thanh ghi như IPSR, PRIMASK, FAULTMASK, BASEPRI và CONTROL
		typedef struct {
			uint32_t ipsr;
			uint32_t primask;
			uint32_t faultmask;
			uint32_t basepri;
			uint32_t control;
		} m3_core_reg_t;

		// Định nghĩa cấu trúc để lưu trữ thông tin về lỗi fatal
		typedef struct {
			int8_t		string[10];
			uint8_t		code;
			task_t		current_task;
			ak_msg_t	current_active_object;
			m3_core_reg_t m3_core_reg;
			uint32_t	restart_times;
		} fatal_log_t;

		// Định nghĩa macro để xử lý lỗi fatal
		#define FATAL(s, c) \
			do { \
				DISABLE_INTERRUPTS(); \
				sys_dbg_fatal((const int8_t*)s, (uint8_t)c); \
			} while (0)
		
		// Định nghĩa macro để xử lý lỗi fatal với điều kiện
		#define FATAL_PARAM(expr, s, c) \
			do { \
				if (!expr) { \
					DISABLE_INTERRUPTS(); \
					sys_dbg_fatal((const int8_t*)s, (uint8_t)c); \
				} \
			} while (0)

		// Khai báo hàm để xử lý lỗi fatal, nhận vào một chuỗi mô tả lỗi và một mã lỗi
		extern void sys_dbg_fatal(const int8_t* s, uint8_t c);

	#ifdef __cplusplus
	}
	#endif

#endif //__SYS_DBG_H__

