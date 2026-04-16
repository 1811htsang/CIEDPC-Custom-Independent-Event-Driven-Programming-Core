// Khai báo thư viện sử dụng
#include <string.h>
#include "ciedpc_core.h"
#include "ciedpc_task.h"
#include "ciedpc_msg.h"
#include "queue.h"

// Khai báo cấu trúc dữ liệu và biến toàn cục cho quản lý task
typedef struct {
	task_pri_t  pri;  		// Mức ưu tiên tác vụ
	uint8_t     mask; 		// Mặt nạ bit để xác định vị trí của tác vụ trong biến task_ready
	ak_msg_t*   qhead;		// Con trỏ tin nhắn đầu tiên trong hàng đợi tác vụ
	ak_msg_t*   qtail;		// Con trỏ tin nhắn cuối cùng trong hàng đợi tác vụ
} tcb_t;

// Biến toàn cục quản lý trạng thái hiện tại của task
static task_id_t current_task_id;     	// ID của tác vụ hiện tại đang được thực thi
static task_norm_t current_task_info;      	// Thông tin chi tiết của tác vụ hiện tại
static ciedpc_msg_t current_active_object;  // Đối tượng tin nhắn hiện tại đang được xử lý

// Biến toàn cục quản lý hàng đợi tác vụ theo mức ưu tiên và bảng thông tin tác vụ
static tcb_t	task_pri_queue[CIEDPC_TASK_PRI_MAX_SIZE];

// Biến toàn cục quản lý bảng thông tin tác vụ và tác vụ polling
static task_norm_t*	task_table = (task_norm_t*)0; // Bảng thông tin tác vụ, được khởi tạo thông qua hàm task_create()
static uint8_t	task_table_size = 0;		 // Kích thước bảng thông tin tác vụ
static uint8_t	task_current = 0;				 // ID của tác vụ hiện tại đang được thực thi, được cập nhật trong quá trình chạy scheduler
static uint8_t	task_ready = 0;					 // Biến bitmask quản lý trạng thái sẵn sàng của các tác vụ

// Biến toàn cục quản lý bảng thông tin tác vụ polling
static task_polling_t* task_polling_table = (task_polling_t*)0; // Bảng thông tin tác vụ polling, được khởi tạo thông qua hàm task_polling_create()
static uint8_t	task_polling_table_size = 0;										// Kích thước bảng thông tin tác vụ polling



