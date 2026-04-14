#ifndef __TASK_LIST_H__
	#define __TASK_LIST_H__

	// Khai báo thư viện sử dụng
	#include "ak.h"
	#include "task.h"

	/**
	 * Khai báo danh sách tác vụ và tác vụ polling của ứng dụng
	 * nhằm để người dùng tự định nghĩa mở rộng các tác vụ và tác vụ polling
	 * theo nhu cầu
	 */
	extern const task_t app_task_table[];
	extern const task_polling_t app_task_polling_table[];

	// Khai báo ID của các tác vụ trong ứng dụng mặc định
	typedef enum app_task_id_t {
		// Tác vụ timer hệ thống
		TASK_TIMER_TICK_ID,
		// Tác vụ hệ thống
		AC_TASK_SYSTEM_ID,
		// Tác vụ giao diện
		AC_TASK_IF_ID,
		// Tác vụ cuối cùng
		AK_TASK_EOT_ID,
	} app_task_id_t;

	// Khai báo ID của các tác vụ polling trong ứng dụng mặc định
	typedef enum app_task_polling_id_t {
		/* EOT polling task ID */
		AK_TASK_POLLING_EOT_ID,
	} app_task_polling_id_t;

	// Khai báo hàm xử lý của các tác vụ trong ứng dụng
	extern void task_if(ak_msg_t*);     // Hàm xử lý của tác vụ giao diện

	/**
	 * Ghi chú:
	 * Hàm task_system chưa được định nghĩa trong core,
	 * sẽ xử lý sau
	 */

#endif //__TASK_LIST_H__

