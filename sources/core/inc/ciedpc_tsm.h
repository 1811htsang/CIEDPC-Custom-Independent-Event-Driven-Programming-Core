#ifndef __TSM_H__
	#define __TSM_H__

	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo các thư viện sử dụng
		#include <stdint.h>
		#include "ak.h"
		#include "message.h"

		// Khai báo các hằng số và macro để quản lý trạng thái null, hàm null, bảng null và tín hiệu null trong máy trạng thái chuyển tiếp (TSM)
		#define TSM_NULL_MSG				((uint8_t)0) 				// Null message
		#define TSM_NULL_STATE			((tsm_state_t)0xFF) // Null state
		#define TSM_NULL_ROUTINE		((tsm_func_f)0) 		// Null routine
		#define TSM_NULL_ON_STATE		((tsm_on_state_f)0) // Null on-state function
		#define TSM_NULL_TABLE			((tsm_tbl_t*)0) 		// Null table

		// Khai báo cấu trúc để tránh cyclic include
		typedef uint8_t tsm_state_t;

		// Khai báo con trỏ hàm để quản lý các hàm xử lý trạng thái và hàm callback khi trạng thái thay đổi trong máy trạng thái chuyển tiếp (TSM)
		typedef void (*tsm_func_f)(ak_msg_t*); // Hàm xử lý trạng thái
		typedef void (*tsm_on_state_f)(tsm_state_t); // Hàm callback khi trạng thái thay đổi

		// Khai báo cấu trúc quản lý thông tin của máy trạng thái chuyển tiếp (TSM)
		typedef struct {
			uint8_t sig;							// Tín hiệu
			tsm_state_t next_state;		// Trạng thái tiếp theo sau khi xử lý tín hiệu
			tsm_func_f tsm_func;			// Hàm xử lý trạng thái
		} tsm_t;

		// Khai báo cấu trúc quản lý thông tin của bảng máy trạng thái chuyển tiếp (TSM)
		typedef struct tsm_tbl_t {
			tsm_state_t state;				// Trạng thái hiện tại của TSM
			tsm_on_state_f on_state;	// Hàm callback sẽ được gọi khi trạng thái thay đổi
			tsm_t** table;				    // Bảng chứa các mảng con trỏ đến các cấu trúc tsm_t, mỗi mảng con tương ứng với một trạng thái cụ thể của TSM, giúp quản lý các tín hiệu và hàm xử lý trạng thái cho từng trạng thái của TSM
		} tsm_tbl_t;

		// Khai báo macro để khởi tạo và chuyển trạng thái trong máy trạng thái chuyển tiếp (TSM)
		#define TSM(t, tbl, s) tsm_init(t, tbl, s)	// Macro để khởi tạo TSM với bảng trạng thái và trạng thái ban đầu
		#define TSM_TRAN(t, s) tsm_tran(t, s)				// Macro để chuyển trạng thái của TSM sang trạng thái mới

		// Khai báo các hàm để quản lý máy trạng thái chuyển tiếp (TSM), bao gồm khởi tạo TSM, chuyển trạng thái và xử lý tin nhắn trong TSM
		void tsm_init(tsm_tbl_t* tsm_tbl, tsm_t** tbl, tsm_state_t state); // Hàm khởi tạo TSM với bảng trạng thái và trạng thái ban đầu
		void tsm_tran(tsm_tbl_t* tsm_tbl, tsm_state_t state);							 // Hàm chuyển trạng thái của TSM sang trạng thái mới
		void tsm_dispatch(tsm_tbl_t* tsm_tbl, ak_msg_t* msg); 						// Hàm xử lý tin nhắn trong TSM, giúp xác định tín hiệu của tin nhắn và thực thi hàm xử lý trạng thái tương ứng, cũng như chuyển trạng thái nếu cần thiết

	#ifdef __cplusplus
	}
	#endif

#endif //__TSM_H__

