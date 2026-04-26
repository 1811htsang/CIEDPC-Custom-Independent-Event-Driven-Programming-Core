/**
 * @file test.c
 * @author Shang Huang
 * @brief Test case 02: TSM đơn giản cho tác vụ truyền data từ Task A sang Task B
 * @version 0.1
 * @date 2026-04-26
 * @copyright MIT License
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test.h"
#include "ciedpc_core.h"
#include "ciedpc_task.h"
#include "ciedpc_msg.h"
#include "ciedpc_timer.h"

/**
 * @brief Khai báo static message queue cho task USR (entry point), task A và task B
 * @attention Mỗi tác vụ sẽ có một hàng đợi tin nhắn riêng biệt với kích thước tối đa là 8 tin nhắn,
 *            có thể điều chỉnh tùy theo nhu cầu của bài test, nhưng cần đảm bảo không vượt quá giới hạn của hệ thống CIEDPC.
 */

static ciedpc_msg_t* usr_q_mem[8];
static ciedpc_msg_t* a_q_mem[8];
static ciedpc_msg_t* b_q_mem[8];

/**
 * @brief Định nghĩa handler cho task USR, task A và task B
 */

void task_usr_handler(ciedpc_msg_t* msg) {
  if (msg->sig == SIG_USR_START) {
    printf("[USR] Received START signal. Sending message to Task A...\n");
    ciedpc_msg_t* msg_to_a = ciedpc_msg_alloc(CIEDPC_TASK_NORM_USR_ID, SIG_USR_START, 0);
    ciedpc_task_post_msg(TASK_NORM_A_ID, msg_to_a);
  }
}

void task_a_handler(ciedpc_msg_t* msg) {
  switch (msg->sig) {
  case SIG_USR_START:
    printf("[Task A] Received START signal from USR. Sending message to Task B...\n");
    const char* data = "Hello from Task A!";
    ciedpc_msg_t* msg_to_b = ciedpc_msg_alloc(CIEDPC_TASK_NORM_USR_ID, SIG_TSK_A_TO_B, sizeof(data));
    ciedpc_msg_set_data(msg_to_b, (const ui8*)data, sizeof(data));
    ciedpc_task_post_msg(TASK_NORM_B_ID, msg_to_b);
    break;
  case SIG_TSK_B_TO_A:
    printf("[Task A] Received message from Task B. Test completed successfully!\n");
    ciedpc_msg_t* stop_msg = ciedpc_msg_alloc(CIEDPC_TASK_NORM_USR_ID, SIG_USR_STOP, 0);
    ciedpc_task_post_msg(CIEDPC_TASK_NORM_USR_ID, stop_msg);
    break;
  default:
    break;
  }
}

void task_b_handler(ciedpc_msg_t* msg) {
  switch (msg->sig) {
  case SIG_TSK_A_TO_B:
    printf("[Task B] Received message from Task A. Sending message back to Task A...\n");
    const char* data = "Hello from Task B!";
    ciedpc_msg_t* msg_to_a = ciedpc_msg_alloc(CIEDPC_TASK_NORM_USR_ID, SIG_TSK_B_TO_A, sizeof(data));
    ciedpc_msg_set_data(msg_to_a, (const ui8*)data, sizeof(data));
    ciedpc_task_post_msg(TASK_NORM_A_ID, msg_to_a);
    break;
  default:
    break;
  }
}

/**
 * @brief Định nghĩa bảng task tổng
 * @attention TASK_USR được sử dụng làm entry point để kích hoạt hệ thống, 
 *            trong khi TASK_A và TASK_B sẽ thực hiện các chức năng chính của bài test
 */
task_norm_t app_task_table[] = {
  { CIEDPC_TASK_NORM_USR_ID,  CIEDPC_TASK_PRI_LEVEL_8, {0}, {0}, task_usr_handler,  {0}, usr_q_mem  },
  { TASK_NORM_A_ID,           CIEDPC_TASK_PRI_LEVEL_6, {0}, {0}, task_a_handler,    {0}, a_q_mem    },
  { TASK_NORM_B_ID,           CIEDPC_TASK_PRI_LEVEL_6, {0}, {0}, task_b_handler,    {0}, b_q_mem    },
  { CIEDPC_TASK_NORM_EOT_ID,  CIEDPC_TASK_PRI_LEVEL_0, {0}, {0}, NULL,              {0}, NULL       }
};

/**
 * @brief Hàm giả lập Tick của hệ thống, được chạy trong một luồng riêng biệt 
 *        để mô phỏng hoạt động của timer phần cứng trong môi trường Linux
 */
void* linux_tick_thread(void* arg) {
  while (1) {
    usleep(1000); // 1ms
    ciedpc_timer_tick();
  }
  return NULL;
}

/**
 * @brief Hàm main để khởi chạy bài test
 */
int main() {
  printf("=== CIEDPC LINUX INTEGRATION TEST ===\n");

  ciedpc_core_init();

  ciedpc_msg_pool_init();
  ciedpc_timer_init();
  ciedpc_task_norm_create(app_task_table);

  pthread_t tick_tid;
  pthread_create(&tick_tid, NULL, linux_tick_thread, NULL);

  ciedpc_msg_t* start_msg = ciedpc_msg_alloc(CIEDPC_TASK_NORM_USR_ID, SIG_USR_START, 0);
  ciedpc_task_post_msg(CIEDPC_TASK_NORM_USR_ID, start_msg);

  while (1) {
    ciedpc_task_scheduler();
    usleep(100); // Sleep để tránh CPU hogging
  }

  return 0;
}