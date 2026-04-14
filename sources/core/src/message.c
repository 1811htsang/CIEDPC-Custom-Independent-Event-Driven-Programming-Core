// Khai báo các thư viện sử dụng
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ak.h"
#include "message.h"
#include "task.h"
#include "sys_dbg.h"

// Khai báo bộ nhớ cho pool tin nhắn thuần túy
static ak_msg_pure_t msg_pure_pool[AK_PURE_MSG_POOL_SIZE]; // Biến đại diện
static ak_msg_t* free_list_pure_msg_pool; // Quản lý con trỏ đến danh sách liên kết của pool tin nhắn thuần túy
static uint32_t free_list_pure_used;			// Quản lý số lượng tin nhắn thuần túy đã được sử dụng từ pool
static uint32_t free_list_pure_used_max;	// Quản lý số lượng tin nhắn thuần túy đã được sử dụng tối đa từ pool

// Khai báo bộ nhớ cho pool tin nhắn thông thường
static ak_msg_common_t msg_common_pool[AK_COMMON_MSG_POOL_SIZE]; // Biến đại diện
static ak_msg_t* free_list_common_msg_pool; // Quản lý con trỏ đến danh sách liên kết của pool tin nhắn thông thường
static uint32_t free_list_common_used;			// Quản lý số lượng tin nhắn thông thường đã được sử dụng từ pool
static uint32_t free_list_common_used_max;	// Quản lý số lượng tin nhắn thông thường đã được sử dụng tối đa từ pool

// Khai báo bộ nhớ cho pool tin nhắn động
static ak_msg_dynamic_t msg_dynamic_pool[AK_DYNAMIC_MSG_POOL_SIZE]; // Biến đại diện
static ak_msg_t* free_list_dynamic_msg_pool; // Quản lý con trỏ đến danh sách liên kết của pool tin nhắn động
static uint32_t free_list_dynamic_used;			// Quản lý số lượng tin nhắn động đã được sử dụng từ pool
static uint32_t free_list_dynamic_used_max;	// Quản lý số lượng tin nhắn động đã được sử dụng tối đa từ pool

// Khai báo các hàm tĩnh để khởi tạo pool tin nhắn
static void pure_msg_pool_init(); 		// Khởi tạo pool tin nhắn thuần túy
static void common_msg_pool_init();		// Khởi tạo pool tin nhắn thông thường
static void dynamic_msg_pool_init();	// Khởi tạo pool tin nhắn động

// Khai báo các hàm tĩnh để giải phóng tin nhắn
static void free_pure_msg(ak_msg_t* msg);			// Giải phóng tin nhắn thuần túy
static void free_common_msg(ak_msg_t* msg);		// Giải phóng tin nhắn thông thường
static void free_dynamic_msg(ak_msg_t* msg);	// Giải phóng tin nhắn động

// Hàm kiểm tra con trỏ tin nhắn có thuộc về pool nào hay không
static uint8_t msg_ptr_belongs_to_pool(const ak_msg_t* msg) {
	uint32_t index = 0;

	for (index = 0; index < AK_PURE_MSG_POOL_SIZE; index++) {
		if (msg == &msg_pure_pool[index].msg_header) {
			return AK_MSG_OK;
		}
	}

	for (index = 0; index < AK_COMMON_MSG_POOL_SIZE; index++) {
		if (msg == &msg_common_pool[index].msg_header) {
			return AK_MSG_OK;
		}
	}

	for (index = 0; index < AK_DYNAMIC_MSG_POOL_SIZE; index++) {
		if (msg == &msg_dynamic_pool[index].msg_header) {
			return AK_MSG_OK;
		}
	}

	return AK_MSG_NG; // Trả về AK_MSG_NG nếu con trỏ tin nhắn không thuộc về bất kỳ pool nào
}

// Hàm khởi tạo pool tin nhắn và chuẩn bị hệ thống tin nhắn để sử dụng
void msg_init() {
	pure_msg_pool_init();
	common_msg_pool_init();
	dynamic_msg_pool_init();
}

// Hàm giải phóng tin nhắn, giảm số lượng tham chiếu và nếu số lượng tham chiếu về 0 thì giải phóng bộ nhớ của tin nhắn
void msg_free(ak_msg_t* msg) {
	uint8_t pool_type;

	msg_dec_ref_count(msg);

	if (get_msg_ref_count(msg) == 0) {

		pool_type = get_msg_type(msg);

		switch (pool_type) {
		case COMMON_MSG_TYPE:
			free_common_msg(msg);
			break;

		case PURE_MSG_TYPE:
			free_pure_msg(msg);
			break;

		case DYNAMIC_MSG_TYPE:
			free_dynamic_msg(msg);
			break;

		default:
			FATAL("MF", 0x20);
			break;
		}
	}
}

// Hàm giải phóng tin nhắn một cách cưỡng bức, bỏ qua việc giảm số lượng tham chiếu và trực tiếp giải phóng bộ nhớ của tin nhắn
void msg_force_free(ak_msg_t* msg) {
	uint8_t pool_type = get_msg_type(msg);

	switch (pool_type) {
	case COMMON_MSG_TYPE:
		free_common_msg(msg);
		break;

	case PURE_MSG_TYPE:
		free_pure_msg(msg);
		break;

	case DYNAMIC_MSG_TYPE:
		free_dynamic_msg(msg);
		break;

	default:
		FATAL("MF", 0x27);
		break;
	}
}

// Hàm tăng số lượng tham chiếu của tin nhắn, giúp theo dõi số lượng tác vụ đang sử dụng tin nhắn để quản lý bộ nhớ hiệu quả
void msg_inc_ref_count(ak_msg_t* msg) {
	if (get_msg_ref_count(msg) < AK_MSG_REF_COUNT_MAX) {
		msg->ref_count++;
	}
	else {
		FATAL("MF", 0x61);
	}
}

// Hàm giảm số lượng tham chiếu của tin nhắn, giúp theo dõi số lượng tác vụ đang sử dụng tin nhắn và giải phóng bộ nhớ khi không còn tác vụ nào sử dụng tin nhắn nữa
void msg_dec_ref_count(ak_msg_t* msg) {
	if (get_msg_ref_count(msg) > 0) {
		msg->ref_count--;
	}
	else {
		FATAL("MF", 0x28);
	}
}

// Hàm cấp phát bộ nhớ cho tin nhắn
void* ak_malloc(size_t size) {
	extern uint32_t __heap_end__;
	static uint8_t* ak_heap = NULL;

	if (ak_heap != NULL) { // trường hợp khác NULL có nghĩa là đã có bộ nhớ được cấp phát trước đó, nên cần kiểm tra xem có đủ bộ nhớ để cấp phát thêm hay không
		if (((uint32_t)ak_heap + size + __AK_MALLOC_CTRL_SIZE) > ((uint32_t)&__heap_end__)) {
			FATAL("ak_malloc", 0x01);
		}
	}

	ak_heap = (uint8_t*)malloc(size);

	if (ak_heap == NULL) {
		FATAL("ak_malloc", 0x02);
	}

	return ak_heap;
}

// Hàm giải phóng bộ nhớ đã cấp phát cho tin nhắn
void ak_free(void* ptr) {
	free(ptr);
}

// Hàm khởi tạo pool tin nhắn thuần túy
void pure_msg_pool_init() {
	uint32_t index;

	entry_critical();

	free_list_pure_msg_pool = (ak_msg_t*)msg_pure_pool;

	for (index = 0; index < AK_PURE_MSG_POOL_SIZE; index++) {
		msg_pure_pool[index].msg_header.src_task_id = 0;
		msg_pure_pool[index].msg_header.des_task_id = 0;
		msg_pure_pool[index].msg_header.sig = 0;
		msg_pure_pool[index].msg_header.if_src_task_id = 0;
		msg_pure_pool[index].msg_header.if_des_task_id = 0;
		msg_pure_pool[index].msg_header.if_src_type = 0;
		msg_pure_pool[index].msg_header.if_des_type = 0;
		msg_pure_pool[index].msg_header.if_sig = 0;
		msg_pure_pool[index].msg_header.ref_count = PURE_MSG_TYPE;
		if (index == (AK_PURE_MSG_POOL_SIZE - 1)) {
			msg_pure_pool[index].msg_header.next = NULL;
		}
		else {
			msg_pure_pool[index].msg_header.next = (ak_msg_t*)&msg_pure_pool[index + 1];
		}
	}

	free_list_pure_used = 0;
	free_list_pure_used_max = 0;

	exit_critical();
}

// Hàm lấy số lượng tin nhắn thuần túy đã được sử dụng từ pool
uint32_t get_pure_msg_pool_used() {
	return free_list_pure_used;
}

// Hàm lấy số lượng tin nhắn thuần túy đã được sử dụng tối đa từ pool
uint32_t get_pure_msg_pool_used_max() {
	return free_list_pure_used_max;
}

// Hàm lấy một tin nhắn thuần túy từ pool
ak_msg_t* get_pure_msg() {
	ak_msg_t* allocate_message;

	entry_critical();

	allocate_message = free_list_pure_msg_pool;

	if (allocate_message == NULL) {
		FATAL("MF", 0x31);
	}
	else {
		free_list_pure_msg_pool = allocate_message->next;

		free_list_pure_used++;
		if (free_list_pure_used >= free_list_pure_used_max) {
			free_list_pure_used_max = free_list_pure_used;
		}
	}

	reset_msg_ref_count(allocate_message);

	allocate_message->ref_count++;
	allocate_message->src_task_id = get_current_task_id();
	allocate_message->des_task_id = 0;
	allocate_message->sig = 0;
	allocate_message->if_src_task_id = 0;
	allocate_message->if_des_task_id = 0;
	allocate_message->if_src_type = 0;
	allocate_message->if_des_type = 0;
	allocate_message->if_sig = 0;

	exit_critical();

	return allocate_message;
}

// Hàm giải phóng một tin nhắn thuần túy
void free_pure_msg(ak_msg_t* msg) {

	entry_critical();

	msg->next = free_list_pure_msg_pool;
	free_list_pure_msg_pool = msg;

	free_list_pure_used--;

	exit_critical();
}

// Hàm khởi tạo pool tin nhắn thông thường
void common_msg_pool_init() {
	uint32_t index;

	entry_critical();

	free_list_common_msg_pool = (ak_msg_t*)msg_common_pool;

	for (index = 0; index < AK_COMMON_MSG_POOL_SIZE; index++) {
		msg_common_pool[index].msg_header.src_task_id = 0;
		msg_common_pool[index].msg_header.des_task_id = 0;
		msg_common_pool[index].msg_header.sig = 0;
		msg_common_pool[index].msg_header.if_src_task_id = 0;
		msg_common_pool[index].msg_header.if_des_task_id = 0;
		msg_common_pool[index].msg_header.if_src_type = 0;
		msg_common_pool[index].msg_header.if_des_type = 0;
		msg_common_pool[index].msg_header.if_sig = 0;
		msg_common_pool[index].msg_header.ref_count = COMMON_MSG_TYPE;
		msg_common_pool[index].len = 0;
		if (index == (AK_COMMON_MSG_POOL_SIZE - 1)) {
			msg_common_pool[index].msg_header.next = NULL;
		}
		else {
			msg_common_pool[index].msg_header.next = (ak_msg_t*)&msg_common_pool[index + 1];
		}
	}

	free_list_common_used = 0;
	free_list_common_used_max = 0;

	exit_critical();
}

// Hàm lấy một tin nhắn thông thường từ pool
uint32_t get_common_msg_pool_used() {
	return free_list_common_used;
}

// Hàm lấy số lượng tin nhắn thông thường đã được sử dụng tối đa từ pool
uint32_t get_common_msg_pool_used_max() {
	return free_list_common_used_max;
}

// Hàm lấy một tin nhắn thông thường từ pool
ak_msg_t* get_common_msg() {
	ak_msg_t* allocate_message;

	entry_critical();

	allocate_message = free_list_common_msg_pool;

	if (allocate_message == NULL) {
		FATAL("MF", 0x21);
	}
	else {
		free_list_common_msg_pool = allocate_message->next;

		free_list_common_used++;
		if (free_list_common_used >= free_list_common_used_max) {
			free_list_common_used_max = free_list_common_used;
		}
	}

	reset_msg_ref_count(allocate_message);

	allocate_message->ref_count++;
	allocate_message->src_task_id = get_current_task_id();
	allocate_message->des_task_id = 0;
	allocate_message->sig = 0;
	allocate_message->if_src_task_id = 0;
	allocate_message->if_des_task_id = 0;
	allocate_message->if_src_type = 0;
	allocate_message->if_des_type = 0;
	allocate_message->if_sig = 0;

	((ak_msg_common_t*)allocate_message)->len = 0;

	exit_critical();

	return allocate_message;
}

// Hàm giải phóng một tin nhắn thông thường
void free_common_msg(ak_msg_t* msg) {

	entry_critical();

	msg->next = free_list_common_msg_pool;
	free_list_common_msg_pool = msg;

	free_list_common_used--;

	exit_critical();
}

// Hàm khởi tạo pool tin nhắn động
uint8_t set_data_common_msg(ak_msg_t* msg, uint8_t* data, uint8_t size) {
	if (get_msg_type(msg) != COMMON_MSG_TYPE) {
		FATAL("MF", 0x23);
	}

	if (size > AK_COMMON_MSG_DATA_SIZE) {
		FATAL("MF", 0x24);
	}

	((ak_msg_common_t*)msg)->len = size;
	memcpy(((ak_msg_common_t*)msg)->data, data, size);

	return AK_MSG_OK;
}

// Hàm lấy dữ liệu từ một tin nhắn thông thường
uint8_t* get_data_common_msg(ak_msg_t* msg) {

	if (get_msg_type(msg) != COMMON_MSG_TYPE) {
		FATAL("MF", 0x26);
	}

	return ((ak_msg_common_t*)msg)->data;
}

// Hàm lấy độ dài dữ liệu từ một tin nhắn thông thường
uint8_t get_data_len_common_msg(ak_msg_t* msg) {

	if (get_msg_type(msg) != COMMON_MSG_TYPE) {
		FATAL("MF", 0x38);
	}

	return ((ak_msg_common_t*)msg)->len;
}

// Hàm khởi tạo pool tin nhắn động
void dynamic_msg_pool_init() {
	uint32_t index;

	entry_critical();

	free_list_dynamic_msg_pool = (ak_msg_t*)msg_dynamic_pool;

	for (index = 0; index < AK_DYNAMIC_MSG_POOL_SIZE; index++) {
		msg_dynamic_pool[index].msg_header.src_task_id = 0;
		msg_dynamic_pool[index].msg_header.des_task_id = 0;
		msg_dynamic_pool[index].msg_header.sig = 0;
		msg_dynamic_pool[index].msg_header.if_src_task_id = 0;
		msg_dynamic_pool[index].msg_header.if_des_task_id = 0;
		msg_dynamic_pool[index].msg_header.if_src_type = 0;
		msg_dynamic_pool[index].msg_header.if_des_type = 0;
		msg_dynamic_pool[index].msg_header.if_sig = 0;
		msg_dynamic_pool[index].msg_header.ref_count = DYNAMIC_MSG_TYPE;
		msg_dynamic_pool[index].len = 0;
		msg_dynamic_pool[index].data = ((uint8_t*)0);
		if (index == (AK_DYNAMIC_MSG_POOL_SIZE - 1)) {
			msg_dynamic_pool[index].msg_header.next = NULL;
		}
		else {
			msg_dynamic_pool[index].msg_header.next = (ak_msg_t*)&msg_dynamic_pool[index + 1];
		}
	}

	free_list_dynamic_used = 0;
	free_list_dynamic_used_max = 0;

	exit_critical();
}

// Hàm lấy số lượng tin nhắn động đã được sử dụng từ pool
uint32_t get_dynamic_msg_pool_used() {
	return free_list_dynamic_used;
}

// Hàm lấy số lượng tin nhắn động đã được sử dụng tối đa từ pool
uint32_t get_dynamic_msg_pool_used_max() {
	return free_list_dynamic_used_max;
}

// Hàm giải phóng một tin nhắn động
void free_dynamic_msg(ak_msg_t* msg) {

	entry_critical();

	msg->next = free_list_dynamic_msg_pool;
	free_list_dynamic_msg_pool = msg;

	ak_free(((ak_msg_dynamic_t*)msg)->data);

	free_list_dynamic_used--;

	exit_critical();
}

// Hàm lấy một tin nhắn động từ pool
ak_msg_t* get_dynamic_msg() {
	ak_msg_t* allocate_message;

	entry_critical();

	allocate_message = free_list_dynamic_msg_pool;

	if (allocate_message == NULL) {
		FATAL("MF", 0x41);
	}
	else {
		free_list_dynamic_msg_pool = allocate_message->next;

		free_list_dynamic_used++;
		if (free_list_dynamic_used >= free_list_dynamic_used_max) {
			free_list_dynamic_used_max = free_list_dynamic_used;
		}
	}

	reset_msg_ref_count(allocate_message);

	allocate_message->ref_count++;
	allocate_message->src_task_id = get_current_task_id();
	allocate_message->des_task_id = 0;
	allocate_message->sig = 0;
	allocate_message->if_src_task_id = 0;
	allocate_message->if_des_task_id = 0;
	allocate_message->if_src_type = 0;
	allocate_message->if_des_type = 0;
	allocate_message->if_sig = 0;

	((ak_msg_dynamic_t*)allocate_message)->len = 0;
	((ak_msg_dynamic_t*)allocate_message)->data = ((uint8_t*)0);

	exit_critical();

	return allocate_message;
}

// Hàm thiết lập dữ liệu cho một tin nhắn động
uint8_t set_data_dynamic_msg(ak_msg_t* msg, uint8_t* data, uint32_t size) {
	if (get_msg_type(msg) != DYNAMIC_MSG_TYPE) {
		FATAL("MF", 0x43);
	}

	((ak_msg_dynamic_t*)msg)->len = size;
	((ak_msg_dynamic_t*)msg)->data = (uint8_t*)ak_malloc(size);
	memcpy(((ak_msg_dynamic_t*)msg)->data, data, size);

	return AK_MSG_OK;
}

// Hàm lấy dữ liệu từ một tin nhắn động
uint8_t* get_data_dynamic_msg(ak_msg_t* msg) {
	if (get_msg_type(msg) != DYNAMIC_MSG_TYPE) {
		FATAL("MF", 0x46);
	}

	return ((ak_msg_dynamic_t*)msg)->data;
}

// Hàm lấy độ dài dữ liệu từ một tin nhắn động
uint32_t get_data_len_dynamic_msg(ak_msg_t* msg) {
	return ((ak_msg_dynamic_t*)msg)->len;
}

// Hàm kiểm tra con trỏ tin nhắn có hợp lệ hay không
uint8_t msg_is_valid_ptr(const ak_msg_t* msg) {
	if (msg == NULL) {
		return AK_MSG_NG;
	}

	return msg_ptr_belongs_to_pool(msg);
}

// Hàm kiểm tra con trỏ tin nhắn có hợp lệ hoặc là NULL hay không
uint8_t msg_is_valid_or_null(const ak_msg_t* msg) {
	if (msg == NULL) {
		return AK_MSG_OK;
	}

	return msg_ptr_belongs_to_pool(msg);
}
