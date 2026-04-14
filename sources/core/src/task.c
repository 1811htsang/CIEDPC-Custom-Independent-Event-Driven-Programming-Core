// Khai báo thư viện sử dụng
#include <string.h>
#include "timer.h"
#include "ak.h"
#include "task.h"
#include "message.h"
#include "timer.h"
#include "task_list.h"
#include "log_queue.h"
#include "utils.h"
#include "sys_ctrl.h"
#include "sys_dbg.h"

// Khai báo cấu trúc dữ liệu và biến toàn cục cho quản lý task
typedef struct {
	task_pri_t  pri;  		// Mức ưu tiên tác vụ
	uint8_t     mask; 		// Mặt nạ bit để xác định vị trí của tác vụ trong biến task_ready
	ak_msg_t*   qhead;		// Con trỏ tin nhắn đầu tiên trong hàng đợi tác vụ
	ak_msg_t*   qtail;		// Con trỏ tin nhắn cuối cùng trong hàng đợi tác vụ
} tcb_t;

// Biến toàn cục quản lý trạng thái hiện tại của task
static task_id_t current_task_id;     	// ID của tác vụ hiện tại đang được thực thi
static task_t current_task_info;      	// Thông tin chi tiết của tác vụ hiện tại
static ak_msg_t current_active_object;  // Đối tượng tin nhắn hiện tại đang được xử lý

// Biến toàn cục quản lý hàng đợi tác vụ theo mức ưu tiên và bảng thông tin tác vụ
static tcb_t	task_pri_queue[TASK_PRI_MAX_SIZE];

// Biến toàn cục quản lý bảng thông tin tác vụ và tác vụ polling
static task_t*	task_table = (task_t*)0; // Bảng thông tin tác vụ, được khởi tạo thông qua hàm task_create()
static uint8_t	task_table_size = 0;		 // Kích thước bảng thông tin tác vụ
static uint8_t	task_current = 0;				 // ID của tác vụ hiện tại đang được thực thi, được cập nhật trong quá trình chạy scheduler
static uint8_t	task_ready = 0;					 // Biến bitmask quản lý trạng thái sẵn sàng của các tác vụ

// Biến toàn cục quản lý bảng thông tin tác vụ polling
static task_polling_t* task_polling_table = (task_polling_t*)0; // Bảng thông tin tác vụ polling, được khởi tạo thông qua hàm task_polling_create()
static uint8_t	task_polling_table_size = 0;										// Kích thước bảng thông tin tác vụ polling

// Khai báo các hàm nội bộ của task manager
static void task_sheduler();

// Hàm kiểm tra tính toàn vẹn của hàng đợi tác vụ, đảm bảo rằng con trỏ đầu và cuối của hàng đợi hợp lệ và nhất quán
static void task_queue_integrity_check(tcb_t* t_tcb, uint8_t err_code) {
	if (msg_is_valid_or_null(t_tcb->qhead) == AK_MSG_NG) {
		FATAL("TK", err_code);
	}

	if (msg_is_valid_or_null(t_tcb->qtail) == AK_MSG_NG) {
		FATAL("TK", err_code);
	}

	if ((t_tcb->qhead == NULL) && (t_tcb->qtail != NULL)) {
		FATAL("TK", err_code);
	}
}

/*
 * Đăng ký bảng task theo cơ chế event-driven.
 * Bảng phải kết thúc bằng AK_TASK_EOT_ID và mỗi phần tử phải có hàm xử lý task.
 */
void task_create(task_t* task_tbl) {
	uint8_t idx = 0;
	if (task_tbl) {
		task_table = task_tbl;
		while (task_tbl[idx].id != AK_TASK_EOT_ID) {
			if (task_tbl[idx].task == (pf_task)0) {
				FATAL("TK", 0x12);
			}

			if (idx >= AK_TASK_EOT_ID) {
				FATAL("TK", 0x13);
			}

			idx++;
		}
		task_table_size = idx;
	}
	else {
		FATAL("TK", 0x01);
	}
}

/*
 * Đăng ký bảng task polling.
 * Bảng phải kết thúc bằng AK_TASK_POLLING_EOT_ID và mỗi phần tử phải có hàm polling.
 */
void task_polling_create(task_polling_t* task_polling_tbl) {
	uint8_t idx = 0;
	if (task_polling_tbl) {
		task_polling_table = task_polling_tbl;
		while (task_polling_tbl[idx].id != AK_TASK_POLLING_EOT_ID) {
			if (task_polling_tbl[idx].task_polling == (pf_task_polling)0) {
				FATAL("TK", 0x14);
			}

			if (idx >= AK_TASK_POLLING_EOT_ID) {
				FATAL("TK", 0x15);
			}

			idx++;
		}
		task_polling_table_size = idx;
	}
	else {
		FATAL("TK", 0x06);
	}
}

/*
 * Đẩy một message AK vào queue của task đích.
 * Hàm này tự cập nhật metadata nguồn/đích của message và đặt cờ task sẵn sàng.
 */
void task_post(task_id_t des_task_id, ak_msg_t* msg) {
	tcb_t* t_tcb;

	if (msg_is_valid_ptr(msg) == AK_MSG_NG) {
		FATAL("TK", 0x0D);
	}

	if (des_task_id >= task_table_size) {
		FATAL("TK", 0x02);
	}

	t_tcb = &task_pri_queue[task_table[des_task_id].pri - 1];

	entry_critical();
	task_queue_integrity_check(t_tcb, 0x0E);

	msg->next = NULL;
	msg->src_task_id = current_task_id;
	msg->des_task_id = des_task_id;
	msg->if_src_task_id = msg->src_task_id;
	msg->if_des_task_id = des_task_id;

	if (t_tcb->qtail == NULL) {
		/* put message to queue */
		t_tcb->qtail = msg;
		t_tcb->qhead = msg;

		/* change status task to ready*/
		task_ready |= t_tcb->mask;
	}
	else {
		if (msg_is_valid_ptr(t_tcb->qtail) == AK_MSG_NG) {
			FATAL("TK", 0x0E);
		}

		if (msg_is_valid_or_null(t_tcb->qtail->next) == AK_MSG_NG) {
			FATAL("TK", 0x0E);
		}

		/* put message to queue */
		t_tcb->qtail->next = msg;
		t_tcb->qtail = msg;
	}

	exit_critical();
}

/**
 * Loại bỏ tất cả tin nhắn có tín hiệu (sig) cụ thể khỏi hàng đợi của tác vụ được chỉ định bởi task_id.
 * Hàm này duyệt qua hàng đợi tin nhắn của tác vụ, kiểm tra từng tin nhắn 
 * để xác định xem nó có phải là tin nhắn cần loại bỏ hay không (dựa trên des_task_id và sig), 
 * và nếu có, nó sẽ loại bỏ tin nhắn đó khỏi hàng đợi và giải phóng bộ nhớ của nó.
 */
uint8_t task_remove_msg(task_id_t task_id, uint8_t sig) {
	tcb_t* t_tcb;
	uint8_t total_rm_msg = 0;

	ak_msg_t* del_msg = NULL; /* MUST-BE initialized NULL */
	ak_msg_t* trace_msg = NULL;
	ak_msg_t* traverse_msg;


	if (task_id >= AK_TASK_EOT_ID) {
		FATAL("TK", 0x05);
	}

	entry_critical();

	/* get task table control */
	t_tcb = &task_pri_queue[task_table[task_id].pri - 1];
	task_queue_integrity_check(t_tcb, 0x0F);

	/* check task queue available */
	if (task_ready & t_tcb->mask) {

		/* get first message of queue */
		traverse_msg = t_tcb->qhead;

		while (traverse_msg != NULL) {
			if (msg_is_valid_ptr(traverse_msg) == AK_MSG_NG) {
				FATAL("TK", 0x0F);
			}

			if (msg_is_valid_or_null(traverse_msg->next) == AK_MSG_NG) {
				FATAL("TK", 0x0F);
			}

			/* check message task id and signal */
			if (traverse_msg->des_task_id == task_id && traverse_msg->sig == sig) {

				/* assign remove message */
				del_msg = traverse_msg;

				if (del_msg == t_tcb->qhead) {
					t_tcb->qhead = traverse_msg->next;
				}
				else if (del_msg != t_tcb->qhead) {
					trace_msg->next = traverse_msg->next;
				}

				/* last message of queue */
				if (del_msg->next == NULL) {
					t_tcb->qtail = trace_msg;

					/* Check if no message exist after remove current message */
					if (t_tcb->qhead == NULL) {

						/* change status of task to inactive */
						task_ready &= ~t_tcb->mask;
					}
				}
			}
			else {
				trace_msg = traverse_msg;
			}

			/* consider the next message */
			traverse_msg = traverse_msg->next;

			/* free the message if it's found */
			if (del_msg != NULL) {
				msg_force_free(del_msg);
				del_msg = NULL;
				total_rm_msg++;
			}
		}
	}

	exit_critical();
	return total_rm_msg;
}

// Hàm post tin nhắn thuần túy (không có dữ liệu) đến một tác vụ đích với tín hiệu cụ thể
void task_post_pure_msg(task_id_t des_task_id, uint8_t sig) {
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, sig);
	task_post(des_task_id, s_msg);
}

// Hàm post tin nhắn thông thường (có dữ liệu nhỏ) đến một tác vụ đích với tín hiệu cụ thể
void task_post_common_msg(task_id_t des_task_id, uint8_t sig, uint8_t* data, uint8_t len) {
	ak_msg_t* s_msg = get_common_msg();
	set_msg_sig(s_msg, sig);
	set_data_common_msg(s_msg, data, len);
	task_post(des_task_id, s_msg);
}

// Hàm post tin nhắn động (có dữ liệu lớn) đến một tác vụ đích với tín hiệu cụ thể
void task_post_dynamic_msg(task_id_t des_task_id, uint8_t sig, uint8_t* data, uint32_t len) {
	ak_msg_t* s_msg = get_dynamic_msg();
	set_msg_sig(s_msg, sig);
	set_data_dynamic_msg(s_msg, data, len);
	task_post(des_task_id, s_msg);
}

// Hàm khởi tạo task manager, thiết lập trạng thái ban đầu cho scheduler, hàng đợi tác vụ, và khởi tạo hệ thống tin nhắn và timer
int task_init() {
	uint8_t pri;
	tcb_t* t_tcb;

	/*
	 * Reset trạng thái scheduler và toàn bộ queue theo từng mức ưu tiên
	 * trước khi chạy runtime AK.
	 * Bắt buộc gọi trước task_create()/task_polling_create()/task_run().
	 */
	/* init task manager variable */
	current_task_id = 0;
	memset(&current_task_info, 0, sizeof(task_t));
	memset(&current_active_object, 0, sizeof(ak_msg_t));
	task_current = 0;
	task_ready = 0;

	/* init kernel queue */
	for (pri = 1; pri <= TASK_PRI_MAX_SIZE; pri++) {
		t_tcb = &task_pri_queue[pri - 1];
		t_tcb->mask     = (1 << (pri - 1));
		t_tcb->qhead    = NULL;
		t_tcb->qtail    = NULL;
		task_queue_integrity_check(t_tcb, 0x10);
	}

	/* message manager must be initial fist */
	msg_init();

	/* init timer manager */
	timer_init();

	return 0;
}

/*
 * Vòng lặp runtime chính của AK.
 * Luôn ưu tiên chạy task có mức ưu tiên cao nhất đang sẵn sàng,
 * sau đó mới chạy nhóm task polling.
 */
int task_run() {
	for (;;) {
		task_sheduler();
		task_polling_run();
	}
}

/*
 * Chạy một vòng scheduler phục vụ debug mà không đi vào vòng lặp vô hạn.
 * Trả về 1 nếu có task sẵn sàng để xử lý, trả về 0 nếu không có task nào trong queue.
 */
int task_debug_run_once() {
	entry_critical();

	if (task_ready == 0) {
		exit_critical();
		return 0;
	}

	exit_critical();

	task_sheduler();
	task_polling_run();

	return 1;
}

int task_sample_run_once() {
	entry_critical();

	if (task_ready == 0) {
		exit_critical();
		return 0;
	}

	exit_critical();

	task_sheduler();

	return 1;
}

/*
 * Deinit task manager cho mục đích test/debug:
 * - xóa liên kết bảng task/polling,
 * - reset queue và trạng thái scheduler,
 * - reset message/timer pool về trạng thái mới khởi tạo.
 */
void task_deinit() {
	uint8_t pri;
	tcb_t* t_tcb;

	entry_critical();

	current_task_id = 0;
	memset(&current_task_info, 0, sizeof(task_t));
	memset(&current_active_object, 0, sizeof(ak_msg_t));

	task_table = (task_t*)0;
	task_table_size = 0;
	task_current = 0;
	task_ready = 0;

	task_polling_table = (task_polling_t*)0;
	task_polling_table_size = 0;

	for (pri = 1; pri <= TASK_PRI_MAX_SIZE; pri++) {
		t_tcb = &task_pri_queue[pri - 1];
		t_tcb->mask = (1 << (pri - 1));
		t_tcb->qhead = NULL;
		t_tcb->qtail = NULL;
	}

	exit_critical();

	msg_init();
	timer_init();
}

// Hàm thiết lập khả năng (ability) cho một tác vụ polling cụ thể, cho phép bật hoặc tắt khả năng thực thi của tác vụ polling dựa trên ID của nó
void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability) {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < AK_TASK_POLLING_EOT_ID) {

		if (__task_polling_table->id == task_polling_id) {

			entry_critical();

			__task_polling_table->ability = ability;

			exit_critical();

			break;
		}

		__task_polling_table++;
	}

	if (__task_polling_table->id == AK_TASK_POLLING_EOT_ID) {
		FATAL("TK", 0x07);
	}
}

// Hàm chạy tất cả các tác vụ polling có khả năng được bật
void task_polling_run() {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < AK_TASK_POLLING_EOT_ID) {

		entry_critical();
		if (__task_polling_table->ability == AK_ENABLE) {

			exit_critical();
			__task_polling_table->task_polling();
		}
		else {
			exit_critical();
		}
		__task_polling_table++;
	}
}

/**
 * Hàm scheduler chính của AK, được gọi trong vòng lặp runtime 
 * để quản lý và thực thi các tác vụ dựa trên trạng thái sẵn sàng của chúng.
 * Scheduler này ưu tiên chạy tác vụ có mức ưu tiên cao nhất đang sẵn sàng, 
 * lấy tin nhắn từ hàng đợi của tác vụ đó, cập nhật thông tin tác vụ hiện tại 
 * và đối tượng tin nhắn đang xử lý, sau đó gọi hàm xử lý của tác vụ với tin nhắn đó.
 * Sau khi thực thi xong, scheduler sẽ kiểm tra và giải phóng tin nhắn nếu cần thiết, 
 * và tiếp tục vòng lặp để tìm tác vụ tiếp theo sẵn sàng để chạy.
 */
void task_sheduler() {
	// Tạo biến cục bộ để lưu ID của tác vụ mới được chọn để chạy, giúp quản lý việc chuyển đổi giữa các tác vụ một cách hiệu quả trong quá trình thực thi scheduler
	uint8_t t_task_new;

	entry_critical(); // Báo bước vào critical section

	uint8_t t_task_current = task_current; // Lưu ID của tác vụ hiện tại vào biến cục bộ để so sánh với tác vụ mới được chọn trong quá trình chạy scheduler

	// task_ready phải có ít nhất một bit được set trước khi gọi LOG2LKUP().
	while ((task_ready != 0) && ((t_task_new = LOG2LKUP(task_ready)) > t_task_current)) {
		
		// Lấy con trỏ đến control block của tác vụ mới được chọn để chạy
		tcb_t* t_tcb = &task_pri_queue[t_task_new];

		// Kiểm tra tính toàn vẹn của hàng đợi tác vụ mới được chọn
		task_queue_integrity_check(t_tcb, 0x11);

		// Kiểm tra xem hàng đợi của tác vụ mới có tin nhắn nào không, nếu không có thì có lỗi trong quản lý hàng đợi và báo lỗi
		if (t_tcb->qhead == NULL) {
			FATAL("TK", 0x11);
		}

		// Lấy tin nhắn đầu tiên trong hàng đợi của tác vụ mới được chọn để xử lý
		ak_msg_t* t_msg = t_tcb->qhead;
		
		// Kiểm tra tính hợp lệ của tin nhắn được lấy từ hàng đợi, nếu tin nhắn không hợp lệ thì có lỗi trong quản lý tin nhắn và báo lỗi
		if (msg_is_valid_ptr(t_msg) == AK_MSG_NG) {
			FATAL("TK", 0x11);
		}

		// Kiểm tra tính toàn vẹn của liên kết tin nhắn trong hàng đợi, đảm bảo rằng con trỏ next của tin nhắn hợp lệ hoặc là NULL
		if (msg_is_valid_or_null(t_msg->next) == AK_MSG_NG) {
			FATAL("TK", 0x11);
		}

		// Cập nhật con trỏ đầu của hàng đợi tác vụ mới để trỏ đến tin nhắn tiếp theo sau tin nhắn hiện tại
		t_tcb->qhead = t_msg->next;

		// Nếu sau khi lấy tin nhắn hiện tại, con trỏ đầu của hàng đợi trở thành NULL, nghĩa là hàng đợi đã hết tin nhắn
		if (t_msg->next == NULL) {
			// Cập nhật con trỏ cuối của hàng đợi thành NULL vì không còn tin nhắn nào trong hàng đợi
			t_tcb->qtail = NULL;
			// Cập nhật trạng thái của tác vụ mới thành không sẵn sàng vì không còn tin nhắn nào để xử lý
			task_ready &= ~t_tcb->mask;
		}

		// Cập nhật ID của tác vụ hiện tại thành ID của tác vụ mới được chọn để chạy
		task_current = t_task_new;

		// Kiểm tra xem ID của tác vụ đích trong tin nhắn có hợp lệ không, nếu không hợp lệ thì có lỗi trong quản lý tin nhắn và báo lỗi
		if (t_msg->des_task_id >= task_table_size) {
			FATAL("TK", 0x09);
		}

		// Kiểm tra xem hàm xử lý của tác vụ đích có hợp lệ không, nếu không hợp lệ thì có lỗi trong quản lý tác vụ và báo lỗi
		if (task_table[t_msg->des_task_id].task == (pf_task)0) {
			FATAL("TK", 0x0A);
		}

		// Cập nhật thông tin của tác vụ hiện tại bằng cách sao chép thông tin từ bảng tác vụ dựa trên ID của tác vụ đích trong tin nhắn
		memcpy(&current_task_info, &task_table[t_msg->des_task_id], sizeof(task_t));
		memcpy(&current_active_object, t_msg, sizeof(ak_msg_t));

		// Cập nhật ID của tác vụ hiện tại thành ID của tác vụ đích trong tin nhắn
		current_task_id = t_msg->des_task_id;

		exit_critical(); // Báo bước ra khỏi critical section

		// Lấy con trỏ đến hàm xử lý của tác vụ đích từ bảng tác vụ dựa trên ID của tác vụ đích trong tin nhắn
		pf_task task_handler = task_table[t_msg->des_task_id].task;

		// Kiểm tra xem hàm xử lý của tác vụ đích có phải là một địa chỉ hợp lệ
		if ((((uint32_t)task_handler) & 0x01U) == 0U) {
			FATAL("TK", 0x08);
		}

		// Gọi hàm xử lý của tác vụ đích với tin nhắn hiện tại làm đối số, thực thi chức năng của tác vụ dựa trên nội dung của tin nhắn
		task_handler(t_msg);

		entry_critical(); // Báo bước vào critical section để tiếp tục quản lý trạng thái sau khi thực thi tác vụ

		// Gọi giải phóng tin nhắn sau khi đã xử lý xong
		msg_free(t_msg);
	}

	// Cập nhật ID của tác vụ hiện tại thành giá trị đã lưu trước đó
	task_current = t_task_current;

	// Nếu không có tác vụ nào sẵn sàng để chạy, đặt ID của tác vụ hiện tại thành AK_TASK_IDLE_ID để biểu thị rằng hệ thống đang ở trạng thái nhàn rỗi
	current_task_id = AK_TASK_IDLE_ID;

	exit_critical(); // Báo bước ra khỏi critical section
}

// Hàm lấy ID của tác vụ hiện tại đang được thực thi
task_id_t get_current_task_id() {
	return current_task_id;
}

// Hàm lấy thông tin chi tiết của tác vụ hiện tại đang được thực thi, bao gồm ID, mức ưu tiên, và hàm thực thi của tác vụ hiện tại
task_t* get_current_task_info() {
	return (task_t*)&current_task_info;
}

// Hàm lấy con trỏ đến đối tượng tin nhắn hiện tại đang được xử lý bởi tác vụ hiện tại
ak_msg_t* get_current_active_object() {
	return (ak_msg_t*)&current_active_object;
}

// Hàm lấy kích thước của bảng thông tin tác vụ, trả về số lượng tác vụ đã được đăng ký trong hệ thống
uint8_t get_task_table_size() {
	return task_table_size;
}


