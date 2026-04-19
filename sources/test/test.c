#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test.h"

/* --- KHAI BÁO BỘ NHỚ CHO TASK (THEO THIẾT KẾ CỦA BẠN) --- */
static ciedpc_msg_t* ctrl_q_mem[8];
static ciedpc_msg_t* blink_q_mem[8];

/* --- LOGIC TASK B (BLINKER) DÙNG TSM --- */
void fn_on_idle(ciedpc_msg_t* msg) { 
  if (msg->sig == CIEDPC_TSM_SIG_ENTRY) printf("[Blinker] -> Entered IDLE Mode\n");
}

void fn_on_active_entry(ciedpc_msg_t* msg) {
  printf("[Blinker] -> Entered ACTIVE Mode. Starting Timer 500ms...\n");
  ciedpc_timer_set(TASK_NORM_BLINKER_ID, SIG_INTERNAL_TICK, 500, CIEDPC_TIMER_PERIODIC);
}

void fn_active_logic(ciedpc_msg_t* msg) {
  if (msg->sig == SIG_INTERNAL_TICK) {
    printf("[Blinker] Processing periodic logic (Data: %d)\n", msg->des_task_id);
  }
}

/* Bảng chuyển trạng thái của Blinker */
const tsm_trans_t blink_idle_trans[] = {
  { SIG_USR_START, STATE_BLINK_ACTIVE, NULL }
};

const tsm_trans_t blink_active_trans[] = {
  { SIG_INTERNAL_TICK, CIEDPC_TSM_STATE_STAY, fn_active_logic },
  { SIG_USR_STOP,      STATE_BLINK_IDLE,      NULL }
};

const tsm_state_desc_t blinker_tsm_table[] = {
  { STATE_BLINK_IDLE,   fn_on_idle,         NULL, blink_idle_trans,   1 },
  { STATE_BLINK_ACTIVE, fn_on_active_entry, NULL, blink_active_trans, 2 }
};

static ciedpc_tsm_t blinker_tsm;

void task_blinker_handler(ciedpc_msg_t* msg) {
  ciedpc_tsm_dispatch(&blinker_tsm, msg);
}

/* --- TASK A (CONTROLLER) --- */
void task_controller_handler(ciedpc_msg_t* msg) {
  if (msg->sig == SIG_USR_START) {
    printf("[Controller] Relay START signal to Blinker\n");
    ciedpc_msg_t* m = ciedpc_msg_alloc(TASK_NORM_BLINKER_ID, SIG_USR_START, 0);
    ciedpc_task_post_msg(TASK_NORM_BLINKER_ID, m);
  }
}

/* --- BẢNG TASK TỔNG --- */
task_norm_t app_task_table[] = {
  { TASK_NORM_CONTROLLER_ID, CIEDPC_TASK_PRI_LEVEL_5, task_controller_handler, ctrl_q_mem, 8 },
  { TASK_NORM_BLINKER_ID,    CIEDPC_TASK_PRI_LEVEL_5, task_blinker_handler,    blink_q_mem, 8 },
  { CIEDPC_TASK_NORM_EOT_ID, 0, NULL, NULL, 0 }
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

  /* 5. Chạy Scheduler (Vòng lặp Kernel) */
  while (1) {
    ciedpc_task_scheduler();
    usleep(100); // Tránh chiếm 100% CPU của Linux
  }

  return 0;
}