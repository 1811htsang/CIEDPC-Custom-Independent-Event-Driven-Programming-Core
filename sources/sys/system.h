#ifndef __SYSTEM_H__
	#define __SYSTEM_H__

	// Khai báo directive C cho C++
	#ifdef __cplusplus
		extern "C"
		{
	#endif

	// Khai báo thư viện sử dụng
	#include <stdint.h>

	// Định nghĩa cấu trúc để lưu trữ thông tin hệ thống
	typedef struct {
		uint32_t flash_used; 					// Lượng flash đã sử dụng
		uint32_t ram_used;						// Lượng RAM đã sử dụng
		uint32_t data_init_size;			// Kích thước dữ liệu đã khởi tạo
		uint32_t data_non_init_size;	// Kích thước dữ liệu chưa khởi tạo
		uint32_t stack_avail;					// Lượng stack còn trống
		uint32_t heap_avail;					// Lượng heap còn trống
		uint32_t ram_other;						// Lượng RAM khác
		uint32_t cpu_clock;						// Tốc độ clock của CPU
		uint32_t tick;								// Giá trị tick
	} system_info_t;

	// Khai báo biến toàn cục để lưu trữ thông tin hệ thống
	extern system_info_t system_info;

	#ifdef __cplusplus
		}
	#endif

#endif
