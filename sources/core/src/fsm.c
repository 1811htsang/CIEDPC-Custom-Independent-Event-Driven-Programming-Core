// Khai báo thư viện sử dụng
#include "fsm.h"

// Định nghĩa hàm để xử lý tin nhắn trong FSM
void fsm_dispatch(fsm_t* me, ak_msg_t* msg) {
	me->state(msg); // Gọi hàm xử lý trạng thái hiện tại của FSM với tin nhắn được truyền vào
}

