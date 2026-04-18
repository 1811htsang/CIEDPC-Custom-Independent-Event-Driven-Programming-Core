/**
 * @file ciedpc_msg.h
 * @author Shang Huang
 * @brief Message definitions and utilities for CIEDPC system
 * @version 0.1
 * @date 2026-04-16
 * 
 * @copyright MIT License
 * 
 */
#ifndef __CIEDPC_MSG_H__
	#define __CIEDPC_MSG_H__

	#ifdef __cplusplus
	extern "C"
	{
	#endif

		/**
		 * @brief Khai báo các thư viện sử dụng
		 */
		#include <stdint.h>
		#include <stdlib.h>
		#include "ciedpc_core.h"

		/**
		 * @brief Định nghĩa các loại Pool tin nhắn (Nội bộ Core sử dụng)
		 */
		typedef enum ciedpc_msg_type_t {
				CIEDPC_MSG_TYPE_BLANK = 0,    /* Không data */
				CIEDPC_MSG_TYPE_NORM,      		/* Data kích thước cố định (Pool) */
				CIEDPC_MSG_TYPE_ALLOC,     		/* Data lớn (Heap/Large Pool) */
				CIEDPC_MSG_TYPE_EXTAL    			/* Tin nhắn từ interface */
		} ciedpc_msg_type_t;

		/**
		 * @brief Cấu trúc quản lý tin nhắn trong hệ thống CIEDPC
		 * @param next: Con trỏ đến tin nhắn tiếp theo trong danh sách liên kết
		 * @param src_task_id: ID của tác vụ nguồn gửi tin nhắn
		 * @param des_task_id: ID của tác vụ đích nhận tin nhắn
		 * @param sig: Tín hiệu của tin nhắn, dùng để xác định loại tin nhắn và hành động cần thực hiện
		 * @param type: Loại Pool tin nhắn, dùng để xác định cách quản lý bộ nhớ cho tin nhắn
		 * @param ref_count: Số lượng tham chiếu đến tin nhắn, dùng để quản lý việc giải phóng bộ nhớ khi tin nhắn được gửi đến nhiều tác vụ
		 * @param data: Con trỏ đến vùng dữ liệu chứa payload của tin nhắn, kích thước và cách quản lý phụ thuộc vào loại Pool tin nhắn
		 * @param interface: Metadata hỗ trợ cho việc quản lý tin nhắn từ các interface, bao gồm thông tin về nguồn và tín hiệu của tin nhắn
		 */
		typedef struct ciedpc_msg_t {
			/* Quản lý danh sách (Linker) */
			struct ciedpc_msg_t* next;

			/* Thông tin điều hướng */
			ui8 src_task_id; 		/* ID Nguồn */
			ui8 des_task_id;	 	/* ID Đích */
			ui8 sig; 						/* Tín hiệu của tin nhắn */
			
			/* Quản lý bộ nhớ & Pool */
			ui8  type;          /* ciedpc_msg_type_t */
			ui8  ref_count;     /* Số lượng tham chiếu (dùng cho broadcast) */

			/* Payload dữ liệu */
			ui8* data;          /* Con trỏ đến vùng dữ liệu */

			/* Metadata hỗ trợ interface */
			struct {
				ui8 if_src_type;
				ui8 if_sig;
			} interface;

			/* Tùy chọn debug */
			#if defined(CIEDPC_DEBUG_FLAG) && (CIEDPC_DEBUG_FLAG & 0x01u)
				ui32 timestamp;   /* Thời điểm tạo tin nhắn */
			#endif
		} ciedpc_msg_t;

		/**
		 * @brief Cấu trúc quản lý tin nhắn từ ngữ cảnh ISR
		 * @param des_task_id: ID của tác vụ đích nhận tin nhắn từ ISR
		 * @param sig: Tín hiệu của tin nhắn từ ISR
		 */
		typedef struct ciedpc_msg_isr_t {
			ui8 des_task_id; 	
			ui8 sig; 					
		} ciedpc_msg_isr_t;

		/**
		 * @brief Hàm khởi tạo Queue tin nhắn
		 */
		void ciedpc_msg_queue_init();

		/**
		 * @brief Hàm cấp phát tin nhắn duy nhất (UMI)
		 * @param des_task_id: ID của tác vụ đích nhận tin nhắn
		 * @param sig: Tín hiệu của tin nhắn
		 * @param size: Kích thước dữ liệu yêu cầu (0 nếu là tin nhắn thuần túy)
		 * @return ciedpc_msg_t*: Con trỏ tin nhắn hoặc NULL nếu hết bộ nhớ
		 */
		ciedpc_msg_t* ciedpc_msg_alloc(ui8 des_task_id, ui8 sig, ui16 size);

		/**
		 * @brief Hàm giải phóng tin nhắn
		 * @param msg chứa mục tiêu cần giải phóng
		 */
		void ciedpc_msg_free(ciedpc_msg_t* msg);

		/**
		 * @brief Tăng số lượng tham chiếu (Dùng khi 1 tin nhắn gửi cho nhiều Task)
		 * @param msg: Tin nhắn cần tăng số lượng tham chiếu
		 */
		void ciedpc_msg_ref_inc(ciedpc_msg_t* msg);

		/**
		 * @brief Giảm số lượng tham chiếu (Dùng khi 1 tin nhắn gửi cho nhiều Task)
		 * @param msg: Tin nhắn cần giảm số lượng tham chiếu
		 */
		void ciedpc_msg_ref_dec(ciedpc_msg_t* msg);

		/**
		 * @brief Sao chép dữ liệu vào tin nhắn
		 * @param msg: Tin nhắn cần sao chép dữ liệu vào
		 * @param data: Con trỏ đến dữ liệu cần sao chép
		 * @param size: Kích thước dữ liệu cần sao chép
		 */
		void ciedpc_msg_set_data(ciedpc_msg_t* msg, const ui8* data, ui16 size);

		/**
		 * @brief Hàm nội bộ để khởi tạo Pool tin nhắn
		 * 
		 * @param tid ID của tác vụ đích nhận tin nhắn từ ISR
		 * @param sig Tín hiệu nhận vào
		 * @return RETR_STAT Trạng thái của việc enqueue tín hiệu vào hàng đợi ISR
		 * @attention Hàm này được thiết kế tách biệt dành cho việc xử lý với ISR 
		 */
		RETR_STAT internal_ciedpc_msg_enqueue_isr_sig(task_id_t tid, ui8 sig);

	#ifdef __cplusplus
	}
	#endif

#endif //__CIEDPC_MSG_H__
