#ifndef __TIMER_H__
	#define __TIMER_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>
		#include "ak.h"
		#include "timer.h"
		#include "task.h"
		#include "message.h"

		// Khai báo đơn vị thời gian cho timer tick
		#define TIMER_TICK					(0x01u)

		// Khai báo các hằng số và macro để quản lý con trỏ timer và trạng thái trả về của các hàm liên quan đến timer
		#define TIMER_MSG_NULL				((ak_timer_t*)0)
		#define TIMER_RET_OK				(1u)
		#define TIMER_RET_NG				(0u)

		// Khai báo directive để quản lý kích thước của pool timer
		#ifndef AK_TIMER_POOL_SIZE
			#define AK_TIMER_POOL_SIZE			(16u)
		#endif

		// Khai báo cấu trúc timer để tránh cyclic include và làm rõ ràng hơn về cấu trúc của timer message
		typedef uint8_t						timer_sig_t;

		// Khai báo cấu trúc quản lý loại của timer
		typedef enum {
			TIMER_ONE_SHOT, // Quản lý loại timer một lần, giúp xác định rằng timer sẽ chỉ kích hoạt một lần và sau đó sẽ tự động hủy bỏ
			TIMER_PERIODIC 	// Quản lý loại timer định kỳ, giúp xác định rằng timer sẽ kích hoạt liên tục theo một khoảng thời gian nhất định cho đến khi được hủy bỏ
		} timer_type_t;

		// Khai báo cấu trúc quản lý thông tin của timer
		typedef struct ak_timer_t {
			struct ak_timer_t*	next;		// Con trỏ đến timer tiếp theo trong pool

			task_id_t			des_task_id;	// ID của tác vụ đích nhận tin nhắn khi timer kích hoạt
			timer_sig_t		sig;				// Tín hiệu được gửi đến tác vụ đích khi timer kích hoạt

			uint32_t			counter;		// Bộ đếm của timer
			uint32_t			period;			// Chu kỳ của timer
		} ak_timer_t;

		// Khai báo các hàm quản lý timer
		extern void timer_init(); // Khởi tạo timer
		extern void timer_tick(uint32_t t); // Cập nhật timer theo tick
		extern void task_timer_tick(ak_msg_t* msg); // Xử lý tick của timer trong tác vụ

		// Khai báo các hàm để quản lý việc thiết lập và hủy bỏ timer, cũng như để lấy thông tin về việc sử dụng pool timer
		extern uint8_t timer_set(task_id_t des_task_id, timer_sig_t sig, uint32_t duty, timer_type_t type); // Thiết lập timer với ID của tác vụ đích, tín hiệu, chu kỳ và loại timer
		extern uint8_t timer_remove_attr(task_id_t des_task_id, timer_sig_t sig); // Hủy bỏ timer dựa trên ID của tác vụ đích và tín hiệu

		// Khai báo các hàm để quản lý việc lấy thông tin về việc sử dụng pool timer, giúp theo dõi số lượng timer đang được sử dụng và số lượng timer đã được sử dụng tối đa trong pool timer
		extern uint32_t get_timer_msg_pool_used(); // Trả về số lượng timer đang được sử dụng trong pool timer, giúp theo dõi việc sử dụng tài nguyên của timer trong hệ thống
		extern uint32_t get_timer_msg_pool_used_max(); // Trả về số lượng timer đã được sử dụng tối đa trong pool timer, giúp theo dõi mức độ sử dụng tài nguyên của timer trong hệ thống và đánh giá hiệu quả của việc quản lý timer

	#ifdef __cplusplus
	}
	#endif

#endif //__TIMER_H__

