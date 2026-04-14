#ifndef __FIFO_H__
	#define __FIFO_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo thư viện sử dụng
		#include <stdint.h>
		#include <string.h>
		#include <stdbool.h>

		// Định nghĩa giá trị trả về của các hàm FIFO
		#define RET_FIFO_OK				(0x01)
		#define RET_FIFO_NG				(0x00)

		// Định nghĩa cấu trúc FIFO
		typedef struct {
			uint32_t tail_index;		// Chỉ số đuôi của FIFO
			uint32_t head_index;		// Chỉ số đầu của FIFO
			uint32_t fill_size; 		// Kích thước đã được điền vào FIFO
			uint32_t buffer_size; 	// Kích thước tổng của FIFO
			uint32_t element_size;	// Kích thước của mỗi phần tử trong FIFO
			uint8_t* buffer;
		} fifo_t;

		// Khai báo các hàm xử lý FIFO
		extern void		fifo_init(fifo_t* fifo, void* buffer, uint32_t buffer_size, uint32_t element_size); // Hàm khởi tạo FIFO
		extern uint32_t	fifo_availble(fifo_t* fifo); // Hàm lấy số lượng phần tử có thể chứa trong FIFO
		extern bool		fifo_is_empty(fifo_t* fifo); // Hàm kiểm tra FIFO có rỗng hay không
		extern bool		fifo_is_full(fifo_t* fifo); // Hàm kiểm tra FIFO có đầy hay không
		extern uint8_t	fifo_put(fifo_t* fifo, void* data); // Hàm thêm phần tử vào FIFO
		extern uint8_t	fifo_get(fifo_t* fifo, void* data); // Hàm lấy phần tử ra khỏi FIFO

	#ifdef __cplusplus
	}
	#endif

#endif //__FIFO_H__
