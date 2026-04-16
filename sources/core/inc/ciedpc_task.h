/**
 * @file ciedpc_task.h
 * @author Shang Huang
 * @brief Task management definitions and utilities for CIEDPC system
 * @version 0.1
 * @date 2026-04-16
 * 
 * @copyright MIT License
 * 
 */
#ifndef __TASK_H__
	#define __TASK_H__

	#ifdef __cplusplus
	extern "C"
	{
	#endif

		/**
     * @brief Khai báo các hằng số và macro cho hệ thống CIEDPC
     */
		#include <stdint.h>
		#include <stdbool.h>
		#include "ciedpc_core.h" 
		#include "queue.h" 

		/**
     * @brief Định nghĩa các hằng số cho ID của tác vụ
     * @attention ID của tác vụ được thiết kế tuân thủ theo encoding `0xDFx`, 
     *            trong đó `x` là một giá trị từ 0 đến 15 (0x0 đến 0xF),
     *            cho phép hệ thống CIEDPC quản lý tối đa 16 tác vụ khác nhau, 
     *            bao gồm các tác vụ timer, giao tiếp, hệ thống, debug, người dùng và trống.
     */
		#define CIEDPC_TASK_MAX_SIZE			  (16u) // 16 tác vụ, từ 0 đến 15
    #define CIEDPC_TASK_TIM_ID				(0xDF0) // Tác vụ timer
    #define CIEDPC_TASK_IF_ID		      (0xDF1) // Tác vụ giao tiếp
    #define CIEDPC_TASK_SYS_ID				(0xDF2) // Tác vụ hệ thống (info + memrp)
    #define CIEDPC_TASK_DBG_ID				(0xDF3) // Tác vụ debug
    #define CIEDPC_TASK_USR_ID				(0xDF4) // Tác vụ người dùng
    #define CIEDPC_TASK_IDLE_ID				(0xDFF) // Tác vụ trống

		/**
     * @brief Định nghĩa các hằng số cho mức độ ưu tiên của tác vụ trong hệ thống CIEDPC
     * @attention Mức độ ưu tiên của tác vụ được thiết kế tuân thủ theo encoding `0xDx`,
     *            trong đó `x` là một giá trị từ 0 đến 15 (0x0 đến 0xF),
     *            cho phép hệ thống CIEDPC quản lý tối đa 16 mức độ ưu tiên khác nhau, 
     *            từ mức 0 (ưu tiên thấp nhất) đến mức 15 (ưu tiên cao nhất).
     */
    #define CIEDPC_TASK_PRI_MAX_SIZE				(16u)   // 16 mức ưu tiên, từ 0 đến 15
    #define CIEDPC_TASK_PRI_LEVEL_0				(0xD0u) // Ưu tiên thấp nhất
    #define CIEDPC_TASK_PRI_LEVEL_1				(0xD1u)
    #define CIEDPC_TASK_PRI_LEVEL_2				(0xD2u)
    #define CIEDPC_TASK_PRI_LEVEL_3				(0xD3u)
    #define CIEDPC_TASK_PRI_LEVEL_4				(0xD4u)
    #define CIEDPC_TASK_PRI_LEVEL_5				(0xD5u)
    #define CIEDPC_TASK_PRI_LEVEL_6				(0xD6u)
    #define CIEDPC_TASK_PRI_LEVEL_7				(0xD7u)
    #define CIEDPC_TASK_PRI_LEVEL_8				(0xD8u)
    #define CIEDPC_TASK_PRI_LEVEL_9				(0xD9u)
    #define CIEDPC_TASK_PRI_LEVEL_10			(0xDAu)
    #define CIEDPC_TASK_PRI_LEVEL_11			(0xDBu)
    #define CIEDPC_TASK_PRI_LEVEL_12			(0xDCu)
    #define CIEDPC_TASK_PRI_LEVEL_13			(0xDDu)
    #define CIEDPC_TASK_PRI_LEVEL_14			(0xDEu)
    #define CIEDPC_TASK_PRI_LEVEL_15			(0xDFu) // Ưu tiên cao nhất

		/**
		 * @brief Khai báo kiểu dữ liệu để quản lý tin nhắn trong hệ thống CIEDPC
		 * @attention `ciedpc_msg_t` được gọi ở đây để thực thi forward declaration, 
		 * 						cho phép sử dụng con trỏ đến `ciedpc_msg_t` trong các khai báo sau này 
		 * 						mà không cần phải định nghĩa chi tiết của `ciedpc_msg_t` tại thời điểm này, 
		 * 						giúp tránh các vấn đề về phụ thuộc lẫn nhau giữa các cấu trúc dữ liệu trong hệ thống CIEDPC.
		 */
		typedef struct ciedpc_msg_t ciedpc_msg_t;

		/**
		 * @brief Định nghĩa các hằng số để quản lý kích thước 
		 * 				của hàng đợi đối tượng tác vụ và hàng đợi ngắt
		 * @attention Trong thiết kế hệ thống thì bắt buộc phải có hàng đợi dành cho ISR 
		 * 						nhằm đảm bảo tính đồng bộ, an toàn và độc lập phần cứng khi xử lý các sự kiện ngắt
		 */
		#define QUEUE_OBJ_SIZE		(512) // Kích thước của hàng đợi đối tượng
		#define QUEUE_ISR_SIZE		(128) // Kích thước của hàng đợi ngắt

		/**
		 * @brief Định nghĩa các kiểu dữ liệu để quản lý ID 
		 * 				và mức độ ưu tiên của tác vụ trong hệ thống CIEDPC
		 */
		typedef ui8	task_pri_t; // Mức ưu tiên của tác vụ
		typedef ui8	task_id_t; 	// ID của tác vụ

		/**
		 * @brief Định nghĩa các kiểu dữ liệu để quản lý hàm thực thi của tác vụ
		 * @attention `pf_task_norm` được sử dụng để quản lý các hàm thực thi của tác vụ theo cơ chế message-driven, 
		 * 								nơi mỗi tác vụ sẽ được kích hoạt và thực thi dựa trên các tin nhắn mà nó nhận được. 
		 *            `pf_task_polling` được sử dụng để quản lý các hàm thực thi của tác vụ theo cơ chế polling-driven, 
		 * 								nơi mỗi tác vụ sẽ được kích hoạt và thực thi liên tục hoặc theo một lịch trình nhất định, 
		 * 								không phụ thuộc vào việc nhận tin nhắn.
		 */
		typedef void (*pf_task_norm)(ciedpc_msg_t*);
		typedef void (*pf_task_polling)(); 

		/**
		 * @brief Định nghĩa cấu trúc để quản lý thông tin của tác vụ message-driven
		 */
		typedef struct task_norm_t {
			task_id_t id;						
			task_pri_t pri;					
			pf_task_norm task_norm;	
		} task_norm_t;

		/**
		 * @brief Định nghĩa cấu trúc để quản lý thông tin của tác vụ polling-driven
		 * @attention `ability` được sử dụng để quản lý khả năng của tác vụ polling, 
		 *            cho phép hệ thống CIEDPC xác định và điều phối việc thực thi của các tác vụ polling 
		 * 						dựa trên khả năng của chúng.
		 */
		typedef struct task_polling_t {
			task_id_t id;									// ID của tác vụ polling
			ui8 ability;									// Khả năng của tác vụ polling
			pf_task_polling task_polling;	// Hàm thực thi của tác vụ polling
		} task_polling_t;

		/**
		 * @brief Hàm khởi tạo hệ thống quản lý tác vụ trong CIEDPC
		 * @attention `ciedpc_task_init` được thiết kế để khởi tạo hệ thống quản lý tác vụ trong CIEDPC,
		 *            bao gồm việc thiết lập các cấu trúc dữ liệu cần thiết, cấu hình các hàng đợi và chuẩn bị môi trường 
		 * 						để quản lý và điều phối các tác vụ trong hệ thống CIEDPC một cách hiệu quả và ổn định.
		 */
		RETR_STAT ciedpc_task_init();

		/**
		 * @brief Hàm tạo tác vụ message-driven trong hệ thống CIEDPC
		 * @param task_table: Con trỏ đến bảng chứa thông tin của các tác vụ message-driven cần tạo     
		 */
		void ciedpc_task_norm_create(task_norm_t* task_table);

		/**
		 * @brief Hàm tạo tác vụ polling-driven trong hệ thống CIEDPC
		 * @param task_table: Con trỏ đến bảng chứa thông tin của các tác vụ polling-driven cần tạo     
		 */
		void ciedpc_task_polling_create(task_polling_t* task_table);

		/**
		 * @brief Hàm gửi tin nhắn từ một tác vụ đến một tác vụ khác trong hệ thống CIEDPC
		 * @param dest_id: ID của tác vụ đích mà tin nhắn sẽ được gửi đến
		 * @param msg: Con trỏ đến cấu trúc tin nhắn cần gửi
		 * @return RETR_STAT: Trả về trạng thái của việc gửi tin nhắn
		 */
		RETR_STAT ciedpc_task_post_msg(task_id_t dest_id, ciedpc_msg_t* msg);

		/**
		 * @brief Hàm đăng ký tín hiệu từ ISR cho tác vụ trong hệ thống CIEDPC
		 * @param dest_id: ID của tác vụ đích mà tín hiệu sẽ được đăng ký
		 * @param sig: Giá trị của tín hiệu cần đăng ký
		 * @return RETR_STAT: Trả về trạng thái của việc đăng ký tín hiệu
		 */
		RETR_STAT ciedpc_task_post_isr(task_id_t dest_id, ui8 sig);

		/**
		 * @brief Hàm lập lịch và thực thi các tác vụ trong hệ thống CIEDPC
		 * @return RETR_STAT: Trả về trạng thái của việc lập lịch và thực thi các tác vụ, 
		 * 				 bao gồm các trạng thái như OK, ERROR, BUSY, TIMEOUT, DONE, NRDY và RDY, giúp người dùng dễ dàng xác định kết quả của việc lập lịch và thực thi các tác vụ trong hệ thống CIEDPC.
		 */
		RETR_STAT ciedpc_task_scheduler(); 

		/**
		 * @brief Hàm lấy ID của tác vụ hiện tại đang được thực thi
		 * @return task_id_t 
		 */
		task_id_t ciedpc_task_get_current_id();

		/**
		 * @brief Hàm lấy tin nhắn hiện tại đang được xử lý bởi tác vụ
		 * @return ciedpc_msg_t* 
		 */
		ciedpc_msg_t* ciedpc_task_get_current_msg();

		/**
		 * @brief Hàm kiểm tra xem tác vụ có sẵn sàng để thực thi hay không
		 * 
		 * @param task_id ID của tác vụ cần kiểm tra
		 * @return true nếu tác vụ sẵn sàng để thực thi
		 * @return false nếu tác vụ không sẵn sàng để thực thi
		 */
		bool ciedpc_task_is_ready(task_id_t task_id);

	#ifdef __cplusplus
	}
	#endif

#endif //__TASK_H__

