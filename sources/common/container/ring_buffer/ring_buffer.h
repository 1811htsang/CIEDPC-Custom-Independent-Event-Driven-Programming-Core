#ifndef __RING_BUFFER_H__
	#define __RING_BUFFER_H__

	// Khai báo directive C cho C++
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo thư viện sử dụng
		#include <stdint.h>
		#include <string.h>
		#include <stdbool.h>

		// Khai báo hằng số trả về
		#define RET_RING_BUFFER_OK				(0x01)
		#define RET_RING_BUFFER_NG				(0x00)

		// Khai báo kiểu dữ liệu
		typedef struct {
			uint16_t tail_index; 		// Chỉ số đuôi của vòng đệm
			uint16_t head_index;		// Chỉ số đầu của vòng đệm
			uint16_t fill_size;			// Kích thước đã sử dụng trong vòng đệm
			uint16_t buffer_size;		// Kích thước tổng của vòng đệm
			uint16_t element_size;	// Kích thước của mỗi phần tử trong vòng đệm
			uint8_t* buffer;				// Con trỏ đến vùng nhớ chứa dữ liệu của vòng đệm
		} ring_buffer_t;

		// Định nghĩa một kiểu dữ liệu đặc biệt cho vòng đệm chứa các ký tự (char)
		typedef struct {
			uint16_t tail_index;		// Chỉ số đuôi của vòng đệm
			uint16_t head_index;		// Chỉ số đầu của vòng đệm
			uint16_t fill_size;			// Kích thước đã sử dụng trong vòng đệm
			uint16_t buffer_size;		// Kích thước tổng của vòng đệm
			uint8_t* buffer;				// Con trỏ đến vùng nhớ chứa dữ liệu của vòng đệm
		} ring_buffer_char_t;

		// Khai báo prototype của các hàm
		extern void		ring_buffer_init(ring_buffer_t* ring_buffer, void* buffer, uint16_t buffer_size, uint16_t element_size); // Hàm khởi tạo vòng đệm
		extern uint16_t	ring_buffer_availble(ring_buffer_t* ring_buffer); // Hàm trả về số lượng phần tử có thể lấy ra từ vòng đệm
		extern bool		ring_buffer_is_empty(ring_buffer_t* ring_buffer); // Hàm kiểm tra xem vòng đệm có rỗng hay không
		extern bool		ring_buffer_is_full(ring_buffer_t* ring_buffer); // Hàm kiểm tra xem vòng đệm có đầy hay không
		extern uint8_t	ring_buffer_put(ring_buffer_t* ring_buffer, void* data); // Hàm thêm một phần tử vào vòng đệm, trả về RET_RING_BUFFER_OK nếu thành công, ngược lại trả về RET_RING_BUFFER_NG
		extern uint8_t	ring_buffer_get(ring_buffer_t* ring_buffer, void* data); // Hàm lấy một phần tử ra khỏi vòng đệm, trả về RET_RING_BUFFER_OK nếu thành công, ngược lại trả về RET_RING_BUFFER_NG

		// Các hàm đặc biệt cho vòng đệm chứa các ký tự (char)
		extern void		ring_buffer_char_init(ring_buffer_char_t* ring_buffer, void* buffer, uint16_t buffer_size); // Hàm khởi tạo vòng đệm chứa char, không cần truyền kích thước phần tử vì đã cố định là 1 byte
		extern uint16_t	ring_buffer_char_availble(ring_buffer_char_t* ring_buffer); // Hàm trả về số lượng ký tự có thể lấy ra từ vòng đệm
		extern bool		ring_buffer_char_is_empty(ring_buffer_char_t* ring_buffer); // Hàm kiểm tra xem vòng đệm chứa char có rỗng hay không
		extern bool		ring_buffer_char_is_full(ring_buffer_char_t* ring_buffer); // Hàm kiểm tra xem vòng đệm chứa char có đầy hay không
		extern void		ring_buffer_char_put(ring_buffer_char_t* ring_buffer, uint8_t c); // Hàm thêm một ký tự vào vòng đệm, không cần trả về giá trị vì đã được thiết kế để luôn thành công (nếu đầy sẽ ghi đè lên phần tử cũ nhất)
		extern uint8_t	ring_buffer_char_get(ring_buffer_char_t* ring_buffer); // Hàm lấy một ký tự ra khỏi vòng đệm, trả về ký tự đã lấy hoặc 0 nếu vòng đệm rỗng

	#ifdef __cplusplus
	}
	#endif

#endif //__RING_BUFFER_H__
