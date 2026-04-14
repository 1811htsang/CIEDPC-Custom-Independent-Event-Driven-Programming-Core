#ifndef __TASK_H__
	#define __TASK_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>
		#include <stdbool.h>
		#include "port.h"			 	// Thư viện quản lý attribute và dynamic allocation
		#include "log_queue.h"  // Thư viện quản lý hàng đợi log

		// Khai báo cấu trúc tin nhắn để tránh cyclic include
		typedef struct ak_msg_t ak_msg_t;

		// Khai báo hằng số để quản lý kích thước của hàng đợi log
		#define LOG_QUEUE_OBJECT_SIZE			(512) // Quản lý kích thước của hàng đợi log đối tượng, giúp xác định số lượng log có thể chứa trong hàng đợi log đối tượng
		#define LOG_QUEUE_IRQ_SIZE				(128) // Quản lý kích thước của hàng đợi log ngắt, giúp xác định số lượng log có thể chứa trong hàng đợi log ngắt

		// Khai báo các kiểu dữ liệu để quản lý tác vụ
		typedef uint8_t	task_pri_t; // Quản lý kiểu dữ liệu cho mức ưu tiên của tác vụ, giúp xác định mức độ ưu tiên của các tác vụ trong hệ thống
		typedef uint8_t	task_id_t; // Quản lý kiểu dữ liệu cho ID của tác vụ, giúp xác định và quản lý các tác vụ trong hệ thống thông qua ID của chúng

			/**
		 	* Ghi chú:
		 	* `task_id_t` có thể được sử dụng để quản lý ID của các tác vụ, bao gồm cả tác vụ ngắt và tác vụ nhận tin, giúp xác định và xử lý các tác vụ trong hệ thống một cách hiệu quả.
		 	*/

		// Khai báo con trỏ hàm để quản lý các hàm thực thi của tác vụ
		typedef void (*pf_task)(ak_msg_t*); // Quản lý các task theo message-driven
		typedef void (*pf_task_polling)(); 	// Quản lý các task theo polling-driven

		/**
		 * Ghi chú:
		 * `pf_task` được sử dụng để quản lý các hàm thực thi của tác vụ 
		 * theo cơ chế message-driven, nơi mỗi tác vụ sẽ được kích hoạt 
		 * và thực thi dựa trên các tin nhắn mà nó nhận được.
		 * 
		 * `pf_task_polling` được sử dụng để quản lý các hàm thực thi của tác vụ 
		 * theo cơ chế polling-driven, nơi mỗi tác vụ sẽ được kích hoạt 
		 * và thực thi liên tục hoặc theo một lịch trình nhất định, 
		 * không phụ thuộc vào việc nhận tin nhắn.
		 */

		// Khai báo cấu trúc quản lý thông tin của tác vụ
		typedef struct {
			task_id_t id;		// Quản lý ID của tác vụ, giúp xác định và quản lý các tác vụ trong hệ thống thông qua ID của chúng
			task_pri_t pri;	// Quản lý mức ưu tiên của tác vụ, giúp xác định mức độ ưu tiên của các tác vụ trong hệ thống
			pf_task task;		// Quản lý hàm thực thi của tác vụ, giúp xác định chức năng và hành vi của tác vụ khi nó được kích hoạt và thực thi dựa trên các tin nhắn mà nó nhận được
		} task_t;

		// Khai báo cấu trúc quản lý thông tin của tác vụ polling
		typedef struct {
			task_id_t id;									// Quản lý ID của tác vụ polling, giúp xác định và quản lý các tác vụ polling trong hệ thống thông qua ID của chúng
			uint8_t ability;							// Quản lý khả năng của tác vụ polling, giúp xác định các khả năng hoặc chức năng mà tác vụ polling có thể thực hiện khi nó được kích hoạt và thực thi liên tục hoặc theo một lịch trình nhất định
			pf_task_polling task_polling;	// Quản lý hàm thực thi của tác vụ polling, giúp xác định chức năng và hành vi của tác vụ polling khi nó được kích hoạt và thực thi liên tục hoặc theo một lịch trình nhất định, không phụ thuộc vào việc nhận tin nhắn
		} task_polling_t;

		// Khai báo cấu trúc quản lý thông tin của ngoại lệ
		typedef struct {
			uint32_t except_number;				// Quản lý số hiệu của ngoại lệ, giúp xác định loại ngoại lệ đã xảy ra trong hệ thống
			uint32_t timestamp;						// Quản lý thời gian xảy ra ngoại lệ, giúp theo dõi và phân tích thời điểm xảy ra ngoại lệ để hỗ trợ việc debug và cải thiện hiệu suất của hệ thống
		} exception_info_t;

		// Khai báo directive để quản lý việc kích hoạt log đối tượng tác vụ
		#if defined(AK_TASK_OBJ_LOG_ENABLE)
			extern log_queue_t log_task_dbg_object_queue;
		#endif

		// Khai báo directive để quản lý việc kích hoạt log ngắt
		#if defined(AK_IRQ_OBJ_LOG_ENABLE)
			extern log_queue_t log_irq_queue;
		#endif

		// Khai báo các hàm để quản lý tác vụ
		extern void task_create(task_t* task_tbl);													// Quản lý việc tạo các tác vụ dựa trên bảng tác vụ được cung cấp
		extern void task_post(task_id_t des_task_id, ak_msg_t* msg);				// Quản lý việc đăng tin nhắn đến một tác vụ đích cụ thể
		extern void task_post_pure_msg(task_id_t des_task_id, uint8_t sig);	// Quản lý việc đăng một tin nhắn thuần túy (chỉ chứa phần tiêu đề) đến một tác vụ đích cụ thể
		extern void task_post_common_msg(
			task_id_t des_task_id, uint8_t sig, 
			uint8_t* data, uint8_t len
		);																																	// Quản lý việc đăng một tin nhắn thông thường (chứa phần tiêu đề và dữ liệu có kích thước cố định) đến một tác vụ đích cụ thể						
		extern void task_post_dynamic_msg(
			task_id_t des_task_id, uint8_t sig, 
			uint8_t* data, uint32_t len
		);																																	// Quản lý việc đăng một tin nhắn động (chứa phần tiêu đề và dữ liệu có kích thước thay đổi được cấp phát tại thời điểm chạy) đến một tác vụ đích cụ thể
		extern uint8_t task_remove_msg(task_id_t task_id, uint8_t sig); 		// Quản lý việc xóa một tin nhắn cụ thể dựa trên ID của tác vụ và tín hiệu của tin nhắn
		
		// Khai báo các hàm quản lý task
		extern int task_init(void); // Quản lý việc khởi tạo hệ thống tác vụ, chuẩn bị các tài nguyên cần thiết cho việc chạy hệ thống tác vụ, bao gồm nhưng không giới hạn ở việc khởi tạo các hàng đợi tin nhắn, cấu trúc dữ liệu quản lý tác vụ, và các tài nguyên khác liên quan đến việc quản lý và thực thi các tác vụ trong hệ thống
		extern int task_run(void);	// Quản lý việc chạy hệ thống tác vụ, bao gồm việc điều phối và thực thi các tác vụ dựa trên các tin nhắn được đăng đến chúng, quản lý lịch trình thực thi của các tác vụ, và đảm bảo rằng các tác vụ được thực thi một cách hiệu quả và đúng theo thiết kế của hệ thống
		extern int task_debug_run_once(void); // Chuẩn bị một hàm đặc biệt để chạy một lần duy nhất trong chế độ debug, cho phép quan sát và phân tích trạng thái của hệ thống tác vụ tại một thời điểm cụ thể, giúp phát hiện và sửa lỗi trong quá trình phát triển và debug hệ thống tác vụ
		extern int task_sample_run_once(void); // Chuẩn bị một hàm đặc biệt để chạy một lần duy nhất trong chế độ sample, cho phép quan sát và phân tích trạng thái của hệ thống tác vụ tại một thời điểm cụ thể, giúp đánh giá hiệu suất và hành vi của hệ thống tác vụ trong các tình huống thực tế hoặc mô phỏng
		extern void task_deinit(void); // Quản lý việc giải phóng các tài nguyên đã được cấp phát cho hệ thống tác vụ, đảm bảo rằng tất cả các tài nguyên như hàng đợi tin nhắn, cấu trúc dữ liệu quản lý tác vụ, và các tài nguyên khác liên quan đến việc quản lý và thực thi các tác vụ được giải phóng một cách an toàn và hiệu quả khi hệ thống tác vụ không còn cần thiết hoặc khi hệ thống đang tắt

		// Khai báo các hàm để quản lý tác vụ polling
		extern void task_polling_create(task_polling_t* task_polling_tbl);								// Quản lý việc tạo các tác vụ polling dựa trên bảng tác vụ polling được cung cấp
		extern void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability);	// Quản lý việc thiết lập khả năng của một tác vụ polling cụ thể dựa trên ID của tác vụ polling và giá trị khả năng được cung cấp
		extern void task_polling_run();																										// Quản lý việc chạy hệ thống tác vụ polling, bắt đầu thực thi các tác vụ polling đã được tạo và quản lý việc chuyển đổi giữa các tác vụ polling dựa trên lịch trình của chúng hoặc theo khả năng của chúng

		// Khai báo các hàm để lấy thông tin về tác vụ hiện tại
		extern task_id_t get_current_task_id(); 			// Quản lý việc lấy ID của tác vụ hiện tại
		extern task_t* get_current_task_info(); 			// Quản lý việc lấy thông tin của tác vụ hiện tại, bao gồm ID, mức ưu tiên, và hàm thực thi của tác vụ hiện tại
		extern ak_msg_t* get_current_active_object(); // Quản lý việc lấy con trỏ đến đối tượng tin nhắn hiện tại đang được xử lý bởi tác vụ hiện tại

		// Khai báo các hàm để lấy kích thước của bảng tác vụ
		extern uint8_t get_task_table_size(); 

	#ifdef __cplusplus
	}
	#endif

#endif //__TASK_H__

