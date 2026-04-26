#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test.h"
#include "ciedpc_core.h"
#include "ciedpc_task.h"
#include "ciedpc_tsm.h"
#include "ciedpc_fsm.h"
#include "ciedpc_msg.h"
#include "ciedpc_timer.h"

/* --- KHAI BÁO BỘ NHỚ CHO TASK (THEO THIẾT KẾ CỦA BẠN) --- */
static ciedpc_msg_t* ctrl_q_mem[8];
static ciedpc_msg_t* blink_q_mem[8];

/* --- LOGIC TASK B (BLINKER) DÙNG TSM --- */
void fn_on_active_exit(ciedpc_msg_t* msg) {
  (void)msg;
  printf("[Blinker] -> Exiting ACTIVE Mode. Stopping Timer...\n");
  // Bắt buộc phải gỡ timer để tránh nhận tin nhắn thừa trong IDLE
  ciedpc_timer_remove(TASK_NORM_BLINKER_ID, SIG_INTERNAL_TICK);
}

void fn_on_idle_entry(ciedpc_msg_t* msg) { 
  (void)msg;
  printf("[Blinker] -> Entered IDLE Mode. Waiting for Start...\n");
}

void fn_on_active_entry(ciedpc_msg_t* msg) {
  (void)msg;
  printf("[Blinker] -> Entered ACTIVE Mode. Starting Timer 1000ms...\n");
  ciedpc_timer_set(TASK_NORM_BLINKER_ID, SIG_INTERNAL_TICK, 1000, CIEDPC_TIMER_PERIODIC);
}

void fn_active_logic(ciedpc_msg_t* msg) {
  (void)msg;
  printf("[Blinker] Processing Tick event. State remains ACTIVE.\n");
}

/* Bảng chuyển trạng thái của Blinker */
const tsm_trans_t blink_idle_trans[] = {
  { SIG_USR_START, STATE_BLINK_ACTIVE, NULL },
  { SIG_USR_STOP,  CIEDPC_TSM_STATE_STAY, NULL } // Đã IDLE rồi thì STOP đứng yên
};

const tsm_trans_t blink_active_trans[] = {
  { SIG_INTERNAL_TICK, CIEDPC_TSM_STATE_STAY, fn_active_logic },
  { SIG_USR_STOP,      STATE_BLINK_IDLE,      NULL },
  { SIG_USR_START,     CIEDPC_TSM_STATE_STAY, NULL } // Đã ACTIVE rồi thì START đứng yên
};

const tsm_state_desc_t blinker_tsm_table[] = {
  { STATE_BLINK_IDLE,   fn_on_idle_entry,  NULL,              blink_idle_trans,   1 },
  { STATE_BLINK_ACTIVE, fn_on_active_entry, fn_on_active_exit, blink_active_trans, 2 }
};

static ciedpc_tsm_t blinker_tsm;

void task_blinker_handler(ciedpc_msg_t* msg) {
  ciedpc_tsm_dispatch(&blinker_tsm, msg);
}

/* --- TASK A (CONTROLLER) --- */
void task_controller_handler(ciedpc_msg_t* msg) {
  /* Chấp nhận cả START và STOP để gửi sang cho Blinker */
  if (msg->sig == SIG_USR_START || msg->sig == SIG_USR_STOP) {
    printf("[Controller] Relaying Signal 0x%02X to Blinker\n", msg->sig);
    ciedpc_msg_t* m = ciedpc_msg_alloc(TASK_NORM_BLINKER_ID, msg->sig, 0);
    if (m) ciedpc_task_post_msg(TASK_NORM_BLINKER_ID, m);
  }
}

/* --- BẢNG TASK TỔNG --- */
task_norm_t app_task_table[] = {
  { TASK_NORM_CONTROLLER_ID, CIEDPC_TASK_PRI_LEVEL_8, {0}, {0}, task_controller_handler, {0}, ctrl_q_mem },
  { TASK_NORM_BLINKER_ID,    CIEDPC_TASK_PRI_LEVEL_6, {0}, {0}, task_blinker_handler,    {0}, blink_q_mem },
  { CIEDPC_TASK_NORM_EOT_ID, CIEDPC_TASK_PRI_LEVEL_0, {0}, {0}, NULL, {0}, NULL }
};

/* --- GIẢ LẬP NHỊP TIM HỆ THỐNG TRÊN LINUX --- */
void* linux_tick_thread(void* arg) {
  while (1) {
    usleep(1000); // 1ms
    ciedpc_timer_tick();
  }
  return NULL;
}

/* --- MAIN TEST --- */
int main() {
  printf("=== CIEDPC LINUX INTEGRATION TEST ===\n");

  /* 0. Init PAL */
  ciedpc_core_init();

  /* 1. Init Core Modules */
  ciedpc_msg_pool_init();
  ciedpc_timer_init();
  ciedpc_task_norm_create(app_task_table);
  
  /* 2. Init TSM cho Blinker */
  ciedpc_tsm_init(&blinker_tsm, blinker_tsm_table, 2, STATE_BLINK_IDLE, NULL);

  /* 3. Tạo luồng Tick giả lập phần cứng */
  pthread_t tick_tid;
  pthread_create(&tick_tid, NULL, linux_tick_thread, NULL);

  /* 4. Giả lập một ngắt từ bên ngoài (ISR Bridge) */
  printf("[System] Simulating External Interrupt: Start Button Pressed...\n");
  ciedpc_task_post_isr(TASK_NORM_CONTROLLER_ID, SIG_USR_START);  

  int stop_flag = 0;

  while (1) {
    ciedpc_task_scheduler();
    usleep(100); // Sleep để tránh CPU hogging
  }

  return 0;
}