#ifndef __FSM_H__
	#define __FSM_H__

	// Khai báo C directive cho C++ 
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		// Khai báo thư viện sử dụng
		#include <stdint.h>

		/* Forward declaration to avoid cyclic include with ak.h/message.h. */
		typedef struct ak_msg_t ak_msg_t;

		// Khai báo macro cho máy trạng thái hữu hạn (FSM)
		#define FSM(me, init_func)		((fsm_t*)me)->state = (state_handler)init_func
		#define FSM_TRAN(me, target)	((fsm_t*)me)->state = (state_handler)target

		// Khai báo con trỏ hàm
		typedef void (*state_handler)(ak_msg_t* msg);

		// Khai báo cấu trúc FSM
		typedef struct {
			state_handler state;
		} fsm_t;

		// Khai báo hàm để xử lý tin nhắn trong FSM
		void fsm_dispatch(fsm_t* me, ak_msg_t* msg);

	#ifdef __cplusplus
	}
	#endif

#endif //__FSM_H__
