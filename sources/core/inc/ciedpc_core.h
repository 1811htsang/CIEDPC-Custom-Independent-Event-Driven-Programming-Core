/**
 * @file ciedpc_core.h
 * @author Shang Huang
 * @brief Core definitions and utilities for CIEDPC system
 * @version 0.1
 * @date 2026-04-16
 * 
 * @copyright MIT License
 * 
 */
#ifndef __CIEDPC_CORE_H__
  #define __CIEDPC_CORE_H__

  #ifdef __cplusplus
  extern "C"
  {
  #endif

    /**
     * @brief Khai báo thư viện sử dụng cho hệ thống CIEDPC
     */
    #include <string.h>

    /**
     * @brief Định nghĩa phiên bản của hệ thống CIEDPC
     */
    #define CIEDPC_VERSION						"0.0.1"

    /**
     * @brief Định nghĩa cờ debug cho hệ thống CIEDPC
     */
    #ifndef CIEDPC_DEBUG_FLAG
      #define CIEDPC_DEBUG_FLAG				(0x00u) // Tắt chế độ debug theo mặc định
    #endif

    /**
     * @brief Định nghĩa các hằng số boolean và trạng thái cơ bản cho hệ thống CIEDPC
     */
    #define CIEDPC_TRUE						  (0x01u)
    #define CIEDPC_FALSE						(0x00u)
    #define CIEDPC_ENABLE						CIEDPC_TRUE
    #define CIEDPC_DISABLE					CIEDPC_FALSE
    #define CIEDPC_FLAG_ON					CIEDPC_TRUE
    #define CIEDPC_FLAG_OFF					CIEDPC_FALSE
    #define CIEDPC_RET_OK						CIEDPC_TRUE
    #define CIEDPC_RET_NG						CIEDPC_FALSE

    /**
     * @brief Định nghĩa các hằng số cho tín hiệu hệ thống và tín hiệu tự định nghĩa của người dùng
     * @attention Tín hiệu tự định nghĩa của người dùng được thiết kế tuân thủ theo encoding `0xFx`,
     *            trong đó `x` là một giá trị từ 0 đến 15 (0x0 đến 0xF), 
     *            cho phép người dùng tạo ra tối đa 16 tín hiệu tùy chỉnh khác nhau.
     */
    #define CIEDPC_USER_DEFINE_SIG				(0xF0u) 

    /**
     * @brief Định nghĩa các attribute đối với compiler
     */
    #define CIEDPC_ATTR_PACKED					__attribute__((packed))     // Đóng gói cấu trúc dữ liệu
    #define CIEDPC_ATTR_ALIGNED(x)			__attribute__((aligned(x))) // Căn chỉnh dữ liệu theo kích thước x
    #define CIEDPC_ATTR_SECTION(x)			__attribute__((section(x))) // Lưu một section cụ thể trong bộ nhớ
    #define CIEDPC_ATTR_WEAK						__attribute__((weak))       // Cho phép được ghi đè bởi một định nghĩa khác
    #define CIEDPC_ATTR_UNUSED					__attribute__((unused))     // Tránh cảnh báo các hàm hoặc biến không được sử dụng 

    /**
     * @brief Định nghĩa các kiểu dữ liệu cơ bản cho hệ thống CIEDPC
     */
    #define u unsigned
		#define ul unsigned long
		#define ui unsigned int
		#define ui8 uint8_t
		#define ui16 uint16_t
		#define ui32 uint32_t

    /**
     * @brief Khai báo các từ khóa viết tắt cho các phạm vi truy cập 
     *        và loại liên kết của biến và hàm trong hệ thống CIEDPC
     * @attention Các từ khóa này được thiết kế để cải thiện tính rõ ràng 
     *            và nhất quán trong việc khai báo các biến và hàm, 
     *            giúp người dùng dễ dàng nhận biết phạm vi truy cập 
     *            và loại liên kết của chúng trong hệ thống CIEDPC.
     */
    #define sta static
		#define ext extern
		#define inl inline
		#define stinl static inline

    /**
     * @brief Định nghĩa kiểu dữ liệu trả về cho các hàm trong hệ thống CIEDPC
     * @attention `RETR_STAT` được thiết kế để quản lý các trạng thái trả về của các hàm trong hệ thống CIEDPC, 
     *            bao gồm các trạng thái như OK, ERROR, BUSY, TIMEOUT, DONE, NRDY và RDY, 
     *            giúp người dùng dễ dàng xác định kết quả của các thao tác và xử lý lỗi một cách hiệu quả.
     */
    typedef enum
		{
			STAT_OK       = 0x00U,
			STAT_ERROR    = 0x01U,
			STAT_BUSY     = 0x02U,
			STAT_TIMEOUT  = 0x03U,
			STAT_DONE     = 0x04U,
			STAT_NRDY     = STAT_ERROR,
			STAT_RDY      = STAT_OK
		} RETR_STAT;

  #ifdef __cplusplus
  }
  #endif

#endif // __CIEDPC_CORE_H__

