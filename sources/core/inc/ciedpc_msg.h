#ifndef __MESSAGE_H__
	#define __MESSAGE_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>
		#include <stdlib.h>
		#include "ak.h"
		#include "port.h"

		// Khai báo hằng số tin nhắn
		#define AK_MSG_NULL ((ak_msg_t*)0) // Quản lý con trỏ null cho tin nhắn
		#define AK_MSG_NG		(0x0u) // Quản lý trạng thái không hợp lệ của tin nhắn
		#define AK_MSG_OK		(0x1u) // Quản lý trạng thái hợp lệ của tin nhắn

		// Khai báo directive quản lý kích thước của pool tin nhắn thông thường
		#ifndef AK_COMMON_MSG_POOL_SIZE
			#define AK_COMMON_MSG_POOL_SIZE		(8u)
		#endif

		// Khai báo directive quản lý kích thước dữ liệu của tin nhắn thông thường
		#ifndef AK_COMMON_MSG_DATA_SIZE
			#define AK_COMMON_MSG_DATA_SIZE		(64u)
		#endif

		// Khai báo directive quản lý kích thước của pool tin nhắn thuần túy
		#ifndef AK_PURE_MSG_POOL_SIZE
			#define AK_PURE_MSG_POOL_SIZE			(32u)
		#endif

		// Khai báo directive quản lý kích thước của pool tin nhắn động
		#ifndef AK_DYNAMIC_MSG_POOL_SIZE
			#define AK_DYNAMIC_MSG_POOL_SIZE	(8u)
		#endif

		// Khai báo directive quản lý kích thước của dữ liệu trong tin nhắn động
		#ifndef AK_DYNAMIC_DATA_POOL_SIZE
			#define AK_DYNAMIC_DATA_POOL_SIZE	(128u)
		#endif

		// Khai báo directive quản lý kích thước của PDU động trong tin nhắn động
		#ifndef AK_DYNAMIC_PDU_SIZE
			#define AK_DYNAMIC_PDU_SIZE				(4u)

			/**
			 * Ghi chú:
			 * PDU (Protocol Data Unit) là đơn vị dữ liệu được truyền trong tin nhắn động, 
			 * có kích thước thay đổi và được cấp phát tại thời điểm chạy.
			 */
		#endif

		// Khai báo các hằng số và macro để quản lý mặt nạ và số lượng tham chiếu của tin nhắn
		#define AK_MSG_TYPE_MASK						(0xC0) // Quản lý mặt nạ để xác định loại tin nhắn (thuần túy, thông thường, động)
		#define AK_MSG_REF_COUNT_MASK				(0x3F) // Quản lý mặt nạ để xác định số lượng tham chiếu của tin nhắn, giúp quản lý bộ nhớ hiệu quả

		#define AK_MSG_REF_COUNT_MAX				(7u)   // Quản lý số lượng tham chiếu tối đa của tin nhắn, giúp đảm bảo rằng tin nhắn không bị sử dụng quá mức và gây ra lỗi

		// Khai báo các macro để quản lý số lượng tham chiếu và loại tin nhắn
		#define get_msg_ref_count(x)		((((ak_msg_t*)x)->ref_count) & AK_MSG_REF_COUNT_MASK) 																	// Trả về số lượng tham chiếu của tin nhắn, giúp theo dõi số lượng tác vụ đang sử dụng tin nhắn để quản lý bộ nhớ hiệu quả
		#define reset_msg_ref_count(x)		((((ak_msg_t*)x)->ref_count) = ((ak_msg_t*)x)->ref_count & (~AK_MSG_REF_COUNT_MASK)) 	// Quản lý việc đặt lại số lượng tham chiếu của tin nhắn về 0, giúp chuẩn bị tin nhắn để sử dụng lại hoặc giải phóng khi không còn cần thiết
		#define get_msg_type(x)				((((ak_msg_t*)x)->ref_count) & AK_MSG_TYPE_MASK)																					// Trả về loại của tin nhắn, giúp xác định loại tin nhắn (thuần túy, thông thường, động) để xử lý phù hợp

		// Khai báo cấu trúc tin nhắn cơ bản, được sử dụng cho cả 3 loại tin nhắn (thuần túy, thông thường, động)
		typedef struct ak_msg_t {
			// Quản lý liên kết tin nhắn trong pool và queue
			struct ak_msg_t*	next; // Quản lý con trỏ đến tin nhắn tiếp theo trong pool hoặc queue, giúp xây dựng danh sách liên kết cho việc quản lý bộ nhớ và hàng đợi tin nhắn

			// Quản lý thông tin cơ bản của tin nhắn, bao gồm ID của tác vụ nguồn và đích, số lượng tham chiếu, tín hiệu, và thông tin giao diện bên ngoài
			uint8_t				src_task_id; 	// Quản lý ID của tác vụ nguồn gởi tin nhắn
			uint8_t				des_task_id; 	// Quản lý ID của tác vụ đích nhận tin nhắn
			uint8_t				ref_count; 	 	// Quản lý số lượng tham chiếu đến tin nhắn, giúp quản lý bộ nhớ hiệu quả
			uint8_t				sig; 					// Quản lý tín hiệu của tin nhắn, giúp xác định loại tin nhắn và cách xử lý nó
			uint8_t				if_src_task_id; // Quản lý ID của tác vụ nguồn gởi tin nhắn qua giao diện bên ngoài
			uint8_t				if_des_task_id; // Quản lý ID của tác vụ đích nhận tin nhắn qua giao diện bên ngoài
			uint8_t				if_src_type; // Quản lý loại nguồn của tin nhắn qua giao diện bên ngoài, giúp xác định nguồn gốc của tin nhắn
			uint8_t				if_des_type; // Quản lý loại đích của tin nhắn qua giao diện bên ngoài, giúp xác định đích đến của tin nhắn
			uint8_t				if_sig; // Quản lý tín hiệu của tin nhắn qua giao diện bên ngoài, giúp xác định loại tin nhắn và cách xử lý nó
		} ak_msg_t;

		// Khai báo cấu trúc quản lý tin nhắn thông thường
		typedef struct {
			ak_msg_t	msg_header; // Quản lý phần tiêu đề của tin nhắn, chứa thông tin cơ bản về tin nhắn như ID của tác vụ nguồn và đích, số lượng tham chiếu, tín hiệu, và thông tin giao diện bên ngoài
			uint8_t		len; // Quản lý độ dài của dữ liệu trong tin nhắn thông thường, giúp xác định kích thước của dữ liệu được chứa trong tin nhắn
			uint8_t		data[AK_COMMON_MSG_DATA_SIZE]; // Quản lý dữ liệu của tin nhắn thông thường, chứa dữ liệu có kích thước cố định được xác định bởi AK_COMMON_MSG_DATA_SIZE
		} ak_msg_common_t;

		// Khai báo cấu trúc quản lý tin nhắn thuần túy
		typedef struct {
			ak_msg_t	msg_header; // Quản lý phần tiêu đề của tin nhắn, chứa thông tin cơ bản về tin nhắn như ID của tác vụ nguồn và đích, số lượng tham chiếu, tín hiệu, và thông tin giao diện bên ngoài

			/**
			 * Ghi chú:
			 * Tin nhắn thuần túy chỉ chứa phần tiêu đề và không chứa dữ liệu,
			 * thường được sử dụng để truyền tín hiệu hoặc
			 * thông tin đơn giản giữa các tác vụ mà không cần dữ liệu đính kèm,
			 * giúp tiết kiệm bộ nhớ và tăng hiệu suất khi chỉ cần truyền thông tin cơ bản.
			 */

		} ak_msg_pure_t;

		// Khai báo cấu trúc quản lý tin nhắn động
		typedef struct {
			ak_msg_t	msg_header; // Quản lý phần tiêu đề của tin nhắn, chứa thông tin cơ bản về tin nhắn như ID của tác vụ nguồn và đích, số lượng tham chiếu, tín hiệu, và thông tin giao diện bên ngoài
			uint32_t	len; 				// Quản lý độ dài của dữ liệu trong tin nhắn động, giúp xác định kích thước của dữ liệu được chứa trong tin nhắn
			uint8_t*	data;				// Quản lý dữ liệu của tin nhắn động, chứa dữ liệu có kích thước thay đổi được cấp phát tại thời điểm chạy, giúp linh hoạt trong việc truyền dữ liệu lớn hoặc không xác định trước kích thước giữa các tác vụ.

			/**
			 * Ghi chú:
			 * Quản lý tin nhắn động, chứa dữ liệu có kích thước thay đổi 
			 * và được cấp phát tại thời điểm chạy. 
			 * Tin nhắn này thường được sử dụng khi cần truyền dữ liệu lớn 
			 * hoặc không xác định trước kích thước dữ liệu giữa các tác vụ.
			 */

		} ak_msg_dynamic_t;

		// Khai báo cấu trúc quản lý tiêu đề của tin nhắn giao diện
		typedef struct {
			uint8_t type;					// Quản lý loại của tin nhắn giao diện, giúp xác định loại tin nhắn giao diện (thuần túy, thông thường, động) để xử lý phù hợp
			uint8_t src_task_id;	// Quản lý ID của tác vụ nguồn gửi tin nhắn giao diện, giúp xác định nguồn gốc của tin nhắn giao diện
			uint8_t des_task_id;	// Quản lý ID của tác vụ đích nhận tin nhắn giao diện, giúp xác định đích đến của tin nhắn giao diện
			uint8_t sig;					// Quản lý tín hiệu của tin nhắn giao diện, giúp xác định loại tin nhắn giao diện và cách xử lý nó
			uint8_t if_src_type;	// Quản lý loại nguồn của tin nhắn giao diện, giúp xác định nguồn gốc của tin nhắn giao diện
			uint8_t if_des_type;	// Quản lý loại đích của tin nhắn giao diện, giúp xác định đích đến của tin nhắn giao diện

			/**
			 * Ghi chú:
			 * `if` nghĩa là interface.
			 */

		} __AK_PACKETED ak_msg_if_header_t;

		// Khai báo cấu trúc quản lý tin nhắn giao diện thuần túy
		typedef struct {
			ak_msg_if_header_t header; // Quản lý phần tiêu đề của tin nhắn giao diện, chứa thông tin về loại tin nhắn giao diện, ID của tác vụ nguồn và đích, tín hiệu, và thông tin giao diện bên ngoài
		} ak_msg_pure_if_t;

		// Khai báo cấu trúc quản lý tin nhắn giao diện thông thường
		typedef struct {
			ak_msg_if_header_t header;							// Quản lý phần tiêu đề của tin nhắn giao diện, chứa thông tin về loại tin nhắn giao diện, ID của tác vụ nguồn và đích, tín hiệu, và thông tin giao diện bên ngoài
			uint8_t len;														// Quản lý độ dài của dữ liệu trong tin nhắn giao diện thông thường, giúp xác định kích thước của dữ liệu được chứa trong tin nhắn giao diện
			uint8_t data[AK_COMMON_MSG_DATA_SIZE];	// Quản lý dữ liệu của tin nhắn giao diện thông thường, chứa dữ liệu có kích thước cố định được xác định bởi AK_COMMON_MSG_DATA_SIZE
		} __AK_PACKETED ak_msg_common_if_t;

		// Khai báo cấu trúc quản lý tin nhắn giao diện động	
		typedef struct {
			ak_msg_if_header_t header;						// Quản lý phần tiêu đề của tin nhắn giao diện, chứa thông tin về loại tin nhắn giao diện, ID của tác vụ nguồn và đích, tín hiệu, và thông tin giao diện bên ngoài
			uint32_t len;													// Quản lý độ dài của dữ liệu trong tin nhắn giao diện động, giúp xác định kích thước của dữ liệu được chứa trong tin nhắn giao diện
			uint8_t *data;												// Quản lý dữ liệu của tin nhắn giao diện động, chứa dữ liệu có kích thước thay đổi được cấp phát tại thời điểm chạy, giúp linh hoạt trong việc truyền dữ liệu lớn hoặc không xác định trước kích thước giữa các tác vụ.
		} __AK_PACKETED ak_msg_dynamic_if_t;

		// Khai báo các macro để quản lý việc thiết lập thông tin của tin nhắn
		#define set_msg_sig(m, s)				(((ak_msg_t*)m)->sig = s) 						// Quản lý tín hiệu của tin nhắn, giúp xác định loại tin nhắn và cách xử lý nó
		#define set_msg_src_task_id(m, t)		(((ak_msg_t*)m)->src_task_id = t) // Quản lý ID của tác vụ nguồn gửi tin nhắn
		#define set_msg_des_task_id(m, t)		(((ak_msg_t*)m)->des_task_id = t) // Quản lý ID của tác vụ đích nhận tin nhắn

		// Khai báo các macro để quản lý việc thiết lập thông tin của tin nhắn giao diện
		#define set_if_src_task_id(m, t)		(((ak_msg_t*)m)->if_src_task_id = t) 	// Quản lý ID của tác vụ nguồn gửi tin nhắn giao diện
		#define set_if_des_task_id(m, t)		(((ak_msg_t*)m)->if_des_task_id = t) 	// Quản lý ID của tác vụ đích nhận tin nhắn giao diện
		#define set_if_src_type(m, t)			(((ak_msg_t*)m)->if_src_type = t) 		 	// Quản lý loại của tác vụ nguồn
		#define set_if_des_type(m, t)			(((ak_msg_t*)m)->if_des_type = t) 			// Quản lý loại của tác vụ đích
		#define set_if_sig(m, s)				(((ak_msg_t*)m)->if_sig = s) 							// Quản lý tín hiệu của tin nhắn giao diện

		// Quản lý dữ liệu của tin nhắn thông thường, giúp thiết lập dữ liệu và kích thước của nó
		#define set_if_data_common_msg(m, d, s) set_data_common_msg(m, d, s)
		
		// Quản lý dữ liệu của tin nhắn động, giúp thiết lập dữ liệu và kích thước của nó
		#define set_if_data_dynamic_msg(m, d, s) set_data_dynamic_msg(m, d, s)

		// Khai báo các hàm để quản lý tin nhắn
		extern void msg_init(); 											// Quản lý việc khởi tạo hệ thống tin nhắn, chuẩn bị các pool tin nhắn và cấu trúc dữ liệu cần thiết để quản lý tin nhắn hiệu quả
		extern void msg_free(ak_msg_t* msg); 					// Quản lý việc giải phóng tin nhắn, kiểm tra số lượng tham chiếu trước khi giải phóng để đảm bảo an toàn bộ nhớ
		extern void msg_force_free(ak_msg_t* msg);  	// Quản lý việc giải phóng tin nhắn mà không kiểm tra số lượng tham chiếu, sử dụng khi cần giải phóng ngay lập tức mà không quan tâm đến số lượng tham chiếu
		extern void msg_inc_ref_count(ak_msg_t* msg); // Quản lý việc tăng số lượng tham chiếu của tin nhắn, giúp theo dõi số lượng tác vụ đang sử dụng tin nhắn để quản lý bộ nhớ hiệu quả
		extern void msg_dec_ref_count(ak_msg_t* msg); // Quản lý việc giảm số lượng tham chiếu của tin nhắn, giúp theo dõi số lượng tác vụ đang sử dụng tin nhắn và giải phóng bộ nhớ khi không còn tác vụ nào sử dụng tin nhắn nữa

		/**
		 * Ghi chú:
		 * Sử dụng `extern` để khai báo và được định nghĩa ở nơi khác.
		 */

		// Khai báo các hàm để quản lý bộ nhớ động, giúp cấp phát và giải phóng bộ nhớ một cách hiệu quả trong hệ thống
		extern void* ak_malloc(size_t); // Quản lý việc cấp phát bộ nhớ động, cho phép cấp phát bộ nhớ với kích thước được xác định tại thời điểm chạy
		extern void ak_free(void*); 		// Quản lý việc giải phóng bộ nhớ động, cho phép giải phóng bộ nhớ đã được cấp phát khi không còn cần thiết nữa

		// Khai báo từ khóa quản lý loại tin nhắn
		#define PURE_MSG_TYPE					(0x80)
		#define COMMON_MSG_TYPE				(0xC0)
		#define DYNAMIC_MSG_TYPE			(0x40)

		// Khai báo các hàm quản lý tin nhắn thuần túy
		extern ak_msg_t* get_pure_msg();							// Quản lý việc lấy một tin nhắn thuần túy từ pool
		extern uint32_t get_pure_msg_pool_used();		  // Quản lý việc lấy số lượng tin nhắn thuần túy đang được sử dụng trong pool
		extern uint32_t get_pure_msg_pool_used_max();	// Quản lý việc lấy số lượng tin nhắn thuần túy đã được sử dụng tối đa trong pool

		// Khai báo các hàm quản lý tin nhắn thông thường
		extern ak_msg_t* get_common_msg();																							// Quản lý việc lấy một tin nhắn thông thường từ pool		
		extern uint32_t get_common_msg_pool_used();																			// Quản lý việc lấy số lượng tin nhắn thông thường đang được sử dụng trong pool
		extern uint32_t get_common_msg_pool_used_max();																	// Quản lý việc lấy số lượng tin nhắn thông thường đã được sử dụng tối đa trong pool
		extern uint8_t set_data_common_msg(ak_msg_t* msg, uint8_t* data, uint8_t size);	// Quản lý việc thiết lập dữ liệu của tin nhắn thông thường
		extern uint8_t* get_data_common_msg(ak_msg_t* msg);															// Quản lý việc lấy dữ liệu của tin nhắn thông thường
		extern uint8_t get_data_len_common_msg(ak_msg_t* msg);													// Quản lý việc lấy độ dài dữ liệu của tin nhắn thông thường

		/**
		 * Ghi chú:
		 * Kích thước dữ liệu không được vượt quá AK_COMMON_MSG_DATA_SIZE để đảm bảo an toàn bộ nhớ
		 * Trả về AK_MSG_OK nếu thiết lập thành công, hoặc AK_MSG_NG nếu kích thước dữ liệu vượt quá giới hạn
		 */

		// Khai báo các hàm quản lý tin nhắn động
		extern ak_msg_t* get_dynamic_msg();																									// Quản lý việc lấy một tin nhắn động từ pool
		extern uint32_t get_dynamic_msg_pool_used();																				// Quản lý việc lấy số lượng tin nhắn động đang được sử dụng trong pool
		extern uint32_t get_dynamic_msg_pool_used_max();																		// Quản lý việc lấy số lượng tin nhắn động đã được sử dụng tối đa trong pool
		extern uint8_t set_data_dynamic_msg(ak_msg_t* msg, uint8_t* data, uint32_t size);		// Quản lý việc thiết lập dữ liệu của tin nhắn động
		extern uint8_t* get_data_dynamic_msg(ak_msg_t* msg);																// Quản lý việc lấy dữ liệu của tin nhắn động
		extern uint32_t get_data_len_dynamic_msg(ak_msg_t* msg);														// Quản lý việc lấy độ dài dữ liệu của tin nhắn động							
		extern uint8_t msg_is_valid_ptr(const ak_msg_t* msg);																// Quản lý việc kiểm tra tính hợp lệ của con trỏ tin nhắn
		extern uint8_t msg_is_valid_or_null(const ak_msg_t* msg);														// Quản lý việc kiểm tra tính hợp lệ hoặc null của con trỏ tin nhắn

		 /**
		  * Ghi chú:
		  * Trả về AK_MSG_OK nếu con trỏ tin nhắn hợp lệ, AK_MSG_NG nếu con trỏ tin nhắn không hợp lệ, và AK_MSG_OK nếu con trỏ tin nhắn là null trong trường hợp của msg_is_valid_or_null
		  */

	#ifdef __cplusplus
	}
	#endif

#endif //__MESSAGE_H__
