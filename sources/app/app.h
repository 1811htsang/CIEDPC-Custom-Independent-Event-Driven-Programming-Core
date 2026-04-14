#ifndef __APP_H__
	#define __APP_H__

	// Khai báo C directive cho C++
	#ifdef __cplusplus
		extern "C"
		{
	#endif

		// Khai báo thư viện sử dụng 
		#include "ak.h"

		// Khai báo tín hiệu cho các interface task
		enum {
			AC_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG, // Tín hiệu tin nhắn thuần túy vào
			AC_IF_PURE_MSG_OUT,											// Tín hiệu tin nhắn thuần túy ra
			AC_IF_COMMON_MSG_IN,										// Tín hiệu tin nhắn chung vào 
			AC_IF_COMMON_MSG_OUT,										// Tín hiệu tin nhắn chung ra
			AC_IF_DYNAMIC_MSG_IN,										// Tín hiệu tin nhắn động vào
			AC_IF_DYNAMIC_MSG_OUT,									// Tín hiệu tin nhắn động ra
		};

		// Khai báo chương trình chính của ứng dụng
		extern int main_app();

	#ifdef __cplusplus
		}
	#endif

#endif //__APP_H__

