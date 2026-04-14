# Giới thiệu

Khu vực này để chứa các phần đã bị loại bỏ hoặc không còn được sử dụng trong hệ thống EDP. Các phần này có thể đã được thay thế bằng các giải pháp mới hơn hoặc không còn phù hợp với kiến trúc hiện tại của hệ thống. Việc giữ lại các phần này có thể giúp cho việc tham khảo và hiểu rõ hơn về quá trình phát triển của hệ thống, cũng như cung cấp thông tin về các tính năng hoặc phương pháp đã từng được sử dụng nhưng sau đó đã bị loại bỏ.

## 1

```c
// Khai báo cấu trúc thông tin app
const app_info_t app_info { \
  APP_MAGIC_NUMBER, \
  APP_VER, \
};

// Khai báo prototype các hàm tĩnh
static uint8_t app_run_edp_init_probe();
```

## 2

```c
#define APP_EDP_PROBE_SIG (0xF1u)
```

## 3

```c
// Khai báo cấu trúc chẩn đoán khởi động ứng dụng
typedef struct {
 uint8_t app_info_valid;
 uint8_t task_table_valid;
 uint8_t polling_table_valid;
 uint8_t task_count;
 uint8_t polling_task_count;
 task_id_t first_invalid_task_id;
 task_id_t first_invalid_polling_id;
 task_pri_t first_invalid_task_pri;
 uint8_t first_invalid_polling_ability;
 int edp_init_ret;
 uint8_t edp_probe_valid;
 uint8_t created_task_table_size;
 uint8_t expected_task_table_size;
 unsigned long pure_pool_used_before_queue_probe;
 unsigned long pure_pool_used_after_queue_probe;
 uint8_t queue_probe_removed_count;
 uint8_t queue_probe_ok;
 const task_t* task_tbl_ptr;
 const task_polling_t* polling_tbl_ptr;
} app_startup_diag_t;

// Khai báo biến toàn cục cho chẩn đoán khởi động ứng dụng
volatile app_startup_diag_t g_app_startup_diag = {
 AK_DISABLE,
 AK_DISABLE,
 AK_DISABLE,
 0,
 0,
 AK_TASK_EOT_ID,
 AK_TASK_POLLING_EOT_ID,
 TASK_PRI_LEVEL_0,
 AK_DISABLE,
 -1,
 AK_DISABLE,
 0,
 0,
 0,
 0,
 0,
 AK_DISABLE,
 (task_t*)0,
 (task_polling_t*)0
};

// Định nghĩa hàm thực hiện kiểm tra khởi chạy core EDP
static uint8_t app_run_edp_init_probe() {
 g_app_startup_diag.edp_probe_valid = AK_DISABLE;
 g_app_startup_diag.created_task_table_size = 0;
 g_app_startup_diag.expected_task_table_size = g_app_startup_diag.task_count;
 g_app_startup_diag.queue_probe_removed_count = 0;
 g_app_startup_diag.queue_probe_ok = AK_DISABLE;

 g_app_startup_diag.pure_pool_used_before_queue_probe = 0;
 g_app_startup_diag.pure_pool_used_after_queue_probe = 0;

 g_app_startup_diag.edp_init_ret = task_init();
 if (g_app_startup_diag.edp_init_ret != 0) {
  return AK_DISABLE;
 }

 task_create((task_t*)app_task_table);
 task_polling_create((task_polling_t*)app_task_polling_table);

 g_app_startup_diag.created_task_table_size = get_task_table_size();
 if (g_app_startup_diag.created_task_table_size != g_app_startup_diag.expected_task_table_size) {
  return AK_DISABLE;
 }

 g_app_startup_diag.pure_pool_used_before_queue_probe = get_pure_msg_pool_used();
 task_post_pure_msg(AC_TASK_SYSTEM_ID, APP_EDP_PROBE_SIG);
 g_app_startup_diag.queue_probe_removed_count = task_remove_msg(AC_TASK_SYSTEM_ID, APP_EDP_PROBE_SIG);
 g_app_startup_diag.pure_pool_used_after_queue_probe = get_pure_msg_pool_used();

 g_app_startup_diag.queue_probe_ok =
   (g_app_startup_diag.queue_probe_removed_count == 1)
   && (g_app_startup_diag.pure_pool_used_before_queue_probe == g_app_startup_diag.pure_pool_used_after_queue_probe)
   ? AK_ENABLE
   : AK_DISABLE;

 g_app_startup_diag.edp_probe_valid = (g_app_startup_diag.queue_probe_ok == AK_ENABLE) ? AK_ENABLE : AK_DISABLE;
 return g_app_startup_diag.edp_probe_valid;
}
```
