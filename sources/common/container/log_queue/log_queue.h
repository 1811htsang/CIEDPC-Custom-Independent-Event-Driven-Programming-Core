#ifndef __LOG_QUEUE_H__
	#define __LOG_QUEUE_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>

		// Khai báo hằng số trả về cho log queue
		#define LOG_QUEUE_RET_OK		(0x00) // Trạng thái thành công khi thực hiện các thao tác trên log queue
		#define LOG_QUEUE_RET_NG		(0x01) // Trạng thái lỗi khi thực hiện các thao tác trên log queue, có thể do queue đầy hoặc rỗng

		// Khai báo con trỏ hàm quản lý đọc và ghi cho log queue
		typedef uint8_t (*q_ctrl)(uint32_t, uint8_t* , uint32_t);

		// Khai báo cấu trúc log queue, quản lý các thuộc tính và trạng thái của queue
		typedef struct {
			uint32_t	base; 		// Địa chỉ cơ sở của queue
			uint32_t	head; 		// Vị trí đầu của queue
			uint32_t	tail;   	// Vị trí cuối của queue
			uint32_t	q_len;		// Độ dài tối đa của queue
			uint32_t	e_len; 	 	// Độ dài của mỗi phần tử trong queue
			q_ctrl		read;   	// Con trỏ hàm để đọc dữ liệu từ queue
			q_ctrl		write;  	// Con trỏ hàm để ghi dữ liệu vào queue
			uint32_t	counter; 	// Bộ đếm số phần tử hiện có trong queue
			uint32_t	end; 			// Vị trí kết thúc của queue, được tính bằng base + (q_len * e_len)
		} log_queue_t;

		// Khai báo các hàm quản lý log queue, bao gồm khởi tạo, thêm phần tử, lấy phần tử và kiểm tra độ dài của queue
		uint8_t		log_queue_init(
			log_queue_t* q, // Con trỏ đến cấu trúc log queue cần khởi tạo
			uint32_t b, 		// Địa chỉ cơ sở của queue, nơi lưu trữ dữ liệu
			uint32_t q_l, 	// Độ dài tối đa của queue, xác định số phần tử có thể chứa trong queue
			uint32_t e_l, 	// Độ dài của mỗi phần tử trong queue, xác định kích thước của dữ liệu được lưu trữ
			q_ctrl f_w, 		// Con trỏ hàm để ghi dữ liệu vào queue, được sử dụng để thêm phần tử vào queue
			q_ctrl f_r			// Con trỏ hàm để đọc dữ liệu từ queue, được sử dụng để lấy phần tử ra khỏi queue
		);
		uint8_t		log_queue_put(log_queue_t* q, void* d); // Hàm add element
		uint8_t		log_queue_get(log_queue_t* q, void* d); // Hàm get element
		uint32_t	log_queue_len(log_queue_t* q);					// Hàm get size của queue

	#ifdef __cplusplus
	}
	#endif

#endif //__LOG_QUEUE_H__
