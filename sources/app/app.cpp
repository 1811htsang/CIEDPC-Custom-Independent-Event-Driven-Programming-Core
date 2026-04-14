// Khai báo thư viện sử dụng
#include <stdint.h>
#include "app.h"
#include "ak.h"
#include "fsm.h"
#include "message.h"
#include "timer.h"
#include "task_if.h"
#include "task_list.h"
#include "sys_ctrl.h"

// Khai báo prototype các hàm tĩnh
static int app_run_edp_two_task_debug();
static void app_task_edp_dbg_a(ak_msg_t* msg);
static void app_task_edp_dbg_b(ak_msg_t* msg);

// Khai báo các macro, enum, struct phục vụ debug EDP hai task
#define APP_EDP_DBG_SIG_START (0xA1u)
#define APP_EDP_DBG_SIG_FORWARD (0xB1u)

// Khai báo enum ID cho các task debug EDP
typedef enum {
	APP_EDP_DBG_TASK_A_ID = 0,
	APP_EDP_DBG_TASK_B_ID,
	APP_EDP_DBG_TASK_EOT_ID
} app_edp_dbg_task_id_t;

// Khai báo cấu trúc debug cho EDP hai task
typedef struct {
	uint8_t init_ret;
	uint8_t created_ret;
	uint8_t first_dispatch_ret;
	uint8_t second_dispatch_ret;
	uint8_t deinit_done;

	uint8_t task_a_rx_count;
	uint8_t task_b_rx_count;
	uint8_t transfer_done;

	uint8_t first_sig_seen;
	uint8_t second_sig_seen;
	uint8_t sample_len;
	char sample_payload[32];
} app_edp_two_task_dbg_t;

// Khai báo biến toàn cục phục vụ debug EDP hai task
volatile app_edp_two_task_dbg_t g_app_edp_two_task_dbg = {
	AK_DISABLE,
	AK_DISABLE,
	AK_DISABLE,
	AK_DISABLE,
	AK_DISABLE,
	0,
	0,
	AK_DISABLE,
	0,
	0,
	0,
	{0}
};

// Khai báo bảng tác vụ và bảng tác vụ polling cho debug EDP hai task
static task_t app_edp_dbg_task_table[] = {
	{APP_EDP_DBG_TASK_A_ID, TASK_PRI_LEVEL_2, app_task_edp_dbg_a},
	{APP_EDP_DBG_TASK_B_ID, TASK_PRI_LEVEL_2, app_task_edp_dbg_b},
	{AK_TASK_EOT_ID, TASK_PRI_LEVEL_0, (pf_task)0}
};

// Bảng tác vụ polling cho debug EDP hai task
// Hiện tại không có task polling nào được định nghĩa
static task_polling_t app_edp_dbg_polling_table[] = {
	{AK_TASK_POLLING_EOT_ID, AK_DISABLE, (pf_task_polling)0}
};

// Định nghĩa hàm thực hiện handler task a debug hai task EDP
static void app_task_edp_dbg_a(ak_msg_t* msg) {
	if (msg == (ak_msg_t*)0) {
		return;
	}

	g_app_edp_two_task_dbg.task_a_rx_count++;
	g_app_edp_two_task_dbg.first_sig_seen = msg->sig;

	if (msg->sig == APP_EDP_DBG_SIG_START) {
		uint8_t sample_data[] = "EDP_MSG_A_TO_B";
		task_post_common_msg(APP_EDP_DBG_TASK_B_ID, APP_EDP_DBG_SIG_FORWARD, sample_data, (uint8_t)(sizeof(sample_data) - 1));
	}
}

// Định nghĩa hàm thực hiện handler task b debug hai task EDP
static void app_task_edp_dbg_b(ak_msg_t* msg) {
	if (msg == (ak_msg_t*)0) {
		return;
	}

	g_app_edp_two_task_dbg.task_b_rx_count++;
	g_app_edp_two_task_dbg.second_sig_seen = msg->sig;

	if (msg->sig == APP_EDP_DBG_SIG_FORWARD) {
		uint8_t len = get_data_len_common_msg(msg);
		uint8_t* data = get_data_common_msg(msg);

		if (len >= sizeof(g_app_edp_two_task_dbg.sample_payload)) {
			len = (uint8_t)(sizeof(g_app_edp_two_task_dbg.sample_payload) - 1);
		}

		memcpy((void*)g_app_edp_two_task_dbg.sample_payload, (const void*)data, len);
		g_app_edp_two_task_dbg.sample_payload[len] = 0;
		g_app_edp_two_task_dbg.sample_len = len;
		g_app_edp_two_task_dbg.transfer_done = AK_ENABLE;
	}
}

// Định nghĩa hàm thực hiện debug core EDP với hai task a,b
static int app_run_edp_two_task_debug() {
	memset((void*)&g_app_edp_two_task_dbg, 0, sizeof(app_edp_two_task_dbg_t));

	if (task_init() != 0) {
		return -10;
	}
	g_app_edp_two_task_dbg.init_ret = AK_ENABLE;

	task_create((task_t*)app_edp_dbg_task_table);
	task_polling_create((task_polling_t*)app_edp_dbg_polling_table);
	g_app_edp_two_task_dbg.created_ret = AK_ENABLE;

	task_post_pure_msg(APP_EDP_DBG_TASK_A_ID, APP_EDP_DBG_SIG_START);

	g_app_edp_two_task_dbg.first_dispatch_ret = (uint8_t)task_debug_run_once();

	g_app_edp_two_task_dbg.second_dispatch_ret = (uint8_t)task_debug_run_once();

	if ((g_app_edp_two_task_dbg.task_a_rx_count == 0)
			|| (g_app_edp_two_task_dbg.task_b_rx_count == 0)
			|| (g_app_edp_two_task_dbg.transfer_done == AK_DISABLE)) {
		task_deinit();
		g_app_edp_two_task_dbg.deinit_done = AK_ENABLE;
		return -11;
	}

	task_deinit();
	g_app_edp_two_task_dbg.deinit_done = AK_ENABLE;

	return 0;
}

// Hàm thực hiện chính của ứng dụng
int main_app() {

	int edp_two_task_ret = app_run_edp_two_task_debug();
	if (edp_two_task_ret != 0) {
	}

	return 0;
}
