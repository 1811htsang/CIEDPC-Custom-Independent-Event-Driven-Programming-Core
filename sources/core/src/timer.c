// Khai báo thư viện sử dụng
#include "timer.h"
#include "sys_dbg.h"
#include "task_list.h"

// Khai báo payload sử dụng cho timer trong ngắt
struct ak_timer_payload_irq_t {
	uint32_t counter;
	uint32_t enable_post_msg;
};

// Khai báo biến toàn cục cho payload của timer trong ngắt
static volatile struct ak_timer_payload_irq_t ak_timer_payload_irq = {0, AK_DISABLE};

// Định nghĩa hằng số và biến toàn cục
static ak_timer_t timer_pool[AK_TIMER_POOL_SIZE]; // Pool chứa các timer message, được quản lý thông qua free list
static ak_timer_t* free_list_timer_pool; 					// Con trỏ đến đầu free list của timer message
static uint32_t free_list_timer_used; 						// Biến đếm số lượng timer message đang được sử dụng
static uint32_t free_list_timer_used_max; 				// Biến đếm số lượng timer message đã sử dụng tối đa
static ak_timer_t* timer_list_head; 							// Con trỏ đến đầu danh sách liên kết của các timer đang hoạt động

// Khai báo các hàm nội bộ (private) để quản lý pool và danh sách timer
static void timer_msg_pool_init();																				// Hàm khởi tạo pool của timer message, thiết lập free list ban đầu
static ak_timer_t* get_timer_msg(); 																			// Hàm lấy một timer message từ pool
static void free_timer_msg(ak_timer_t* msg); 															// Hàm trả lại một timer message vào pool
static uint8_t timer_remove_msg(task_id_t des_task_id, timer_sig_t sig); 	// Hàm xóa một timer message khỏi danh sách liên kết dựa trên des_task_id và sig

void timer_msg_pool_init() {
	uint32_t index;

	entry_critical();

	timer_list_head = TIMER_MSG_NULL;
	free_list_timer_pool = (ak_timer_t*)timer_pool;

	for (index = 0; index < AK_TIMER_POOL_SIZE; index++) {
		/* Zero initialize all fields to prevent garbage values */
		timer_pool[index].des_task_id = 0;
		timer_pool[index].sig = 0;
		timer_pool[index].counter = 0;
		timer_pool[index].period = 0;

		if (index == (AK_TIMER_POOL_SIZE - 1)) {
			timer_pool[index].next = TIMER_MSG_NULL;
		}
		else {
			timer_pool[index].next = (ak_timer_t*)&timer_pool[index + 1];
		}
	}

	free_list_timer_used = 0;
	free_list_timer_used_max = 0;

	exit_critical();
}

ak_timer_t* get_timer_msg() {
	ak_timer_t* allocate_timer;

	entry_critical();

	allocate_timer = free_list_timer_pool;

	if (allocate_timer == TIMER_MSG_NULL) {
		FATAL("MT", 0x30);
	}
	else {
		free_list_timer_pool = allocate_timer->next;

		free_list_timer_used++;
		if (free_list_timer_used >= free_list_timer_used_max) {
			free_list_timer_used_max = free_list_timer_used;
		}
	}

	exit_critical();

	return allocate_timer;
}

void free_timer_msg(ak_timer_t* msg) {

	entry_critical();

	msg->next = free_list_timer_pool;
	free_list_timer_pool = msg;

	free_list_timer_used--;

	exit_critical();
}

uint32_t get_timer_msg_pool_used() {
	return free_list_timer_used;
}

uint32_t get_timer_msg_pool_used_max() {
	return free_list_timer_used_max;
}

void task_timer_tick(ak_msg_t* msg) {
	ak_msg_t* timer_msg;

	ak_timer_t* timer_list;
	ak_timer_t* timer_del = TIMER_MSG_NULL; /* MUST-BE assign TIMER_MSG_NULL */

	uint32_t temp_counter;
	uint32_t irq_counter;

	entry_critical();

	timer_list = timer_list_head;

	irq_counter = ak_timer_payload_irq.counter;

	ak_timer_payload_irq.counter = 0;
	ak_timer_payload_irq.enable_post_msg = AK_ENABLE;

	exit_critical();

	switch (msg->sig) {
	case TIMER_TICK:
		while (timer_list != TIMER_MSG_NULL) {

			entry_critical();

			if (irq_counter < timer_list->counter) {
				timer_list->counter -= irq_counter;
			}
			else {
				timer_list->counter = 0;
			}

			temp_counter = timer_list->counter;

			exit_critical();

			if (temp_counter == 0) {

				timer_msg = get_pure_msg();
				set_msg_sig(timer_msg, timer_list->sig);
				task_post(timer_list->des_task_id, timer_msg);

				entry_critical();

				if (timer_list->period) {
					timer_list->counter = timer_list->period;
				}
				else {
					timer_del = timer_list;
				}

				exit_critical();
			}

			timer_list = timer_list->next;

			if (timer_del) {
				timer_remove_msg(timer_del->des_task_id, timer_del->sig);
				timer_del = TIMER_MSG_NULL;
			}
		}
		break;

	default:
		break;
	}
}

void timer_init() {
	timer_msg_pool_init();

	entry_critical();

	ak_timer_payload_irq.counter = 0;
	ak_timer_payload_irq.enable_post_msg = AK_ENABLE;

	exit_critical();
}

void timer_tick(uint32_t t) {
	if (timer_list_head != TIMER_MSG_NULL) {
		ak_timer_payload_irq.counter += t;

		if (ak_timer_payload_irq.enable_post_msg == AK_ENABLE) {
			ak_timer_payload_irq.enable_post_msg = AK_DISABLE;

			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, TIMER_TICK);
			task_post(TASK_TIMER_TICK_ID, s_msg);
		}
	}
}

uint8_t timer_set(
	task_id_t des_task_id, timer_sig_t sig, 
	uint32_t duty, timer_type_t type
) {
	ak_timer_t* timer_msg;

	/* Validate des_task_id before processing */
	if (des_task_id >= get_task_table_size()) {
		FATAL("TK", 0x0B);
	}

	entry_critical();

	timer_msg = timer_list_head;

	while (timer_msg != TIMER_MSG_NULL) {
		if (timer_msg->des_task_id == des_task_id &&
				timer_msg->sig == sig) {

			timer_msg->counter = duty;

			exit_critical();

			return TIMER_RET_OK;
		}
		else {
			timer_msg = timer_msg->next;
		}
	}

	timer_msg = get_timer_msg();

	timer_msg->des_task_id = des_task_id;
	timer_msg->sig = sig;
	timer_msg->counter = duty;

	if (type == TIMER_PERIODIC) {
		timer_msg->period = duty;
	}
	else {
		timer_msg->period = 0;
	}

	if (timer_list_head == TIMER_MSG_NULL) {
		timer_msg->next = TIMER_MSG_NULL;
		timer_list_head = timer_msg;
	}
	else {
		timer_msg->next = timer_list_head;
		timer_list_head = timer_msg;
	}

	exit_critical();

	return TIMER_RET_OK;
}

uint8_t timer_remove_msg(task_id_t des_task_id, timer_sig_t sig) {
	ak_timer_t* timer_msg;
	ak_timer_t* timer_msg_prev;

	/* Validate des_task_id before processing */
	if (des_task_id >= get_task_table_size()) {
		FATAL("TK", 0x0C);
	}

	entry_critical();

	timer_msg = timer_list_head;
	timer_msg_prev = timer_msg;

	while (timer_msg != TIMER_MSG_NULL) {

		if (timer_msg->des_task_id == des_task_id &&
				timer_msg->sig == sig) {

			if (timer_msg == timer_list_head) {
				timer_list_head = timer_msg->next;
			}
			else {
				timer_msg_prev->next = timer_msg->next;
			}

			free_timer_msg(timer_msg);

			exit_critical();

			return TIMER_RET_OK;
		}
		else {
			timer_msg_prev = timer_msg;
			timer_msg = timer_msg->next;
		}
	}

	exit_critical();

	return TIMER_RET_NG;
}

uint8_t timer_remove_attr(task_id_t des_task_id, timer_sig_t sig) {

	uint8_t ret = timer_remove_msg(des_task_id, sig);

	task_remove_msg(des_task_id, sig);

	return ret;
}
