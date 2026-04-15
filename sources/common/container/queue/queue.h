#ifndef __QUEUE_H__
	#define __QUEUE_H__

	#ifdef __cplusplus
	extern "C"
	{
	#endif

		/**
     * @brief Khai báo thư viện sử dụng cho queue implementation
     */
		#include <stdint.h>

		/**
     * @brief Định nghĩa các hằng số trả về cho queue
     */
		#define QUEUE_RET_OK		(0x00u)
		#define QUEUE_RET_NG		(0x01u)

		/**
     * @brief Khai báo con trỏ hàm để quản lý các hàm đọc và ghi dữ liệu trong queue
		 * @param uint32_t: Địa chỉ của phần tử trong queue
		 * @param uint8_t*: Con trỏ đến dữ liệu cần đọc hoặc ghi
		 * @param uint32_t: Kích thước của dữ liệu cần đọc hoặc ghi
		 * @return uint8_t: Trả về trạng thái của thao tác đọc hoặc ghi
     */
		typedef uint8_t (*q_ctrl)(uint32_t, uint8_t* , uint32_t);

		/**
		 * @brief Định nghĩa cấu trúc quản lý thông tin của log queue
		 * @param base: Địa chỉ cơ sở của queue
		 * @param head: Vị trí đầu của queue
		 * @param tail: Vị trí cuối của queue
		 * @param q_len: Độ dài tối đa của queue
		 * @param e_len: Độ dài của mỗi phần tử trong queue
		 * @param read: Con trỏ hàm để đọc dữ liệu từ queue
		 * @param write: Con trỏ hàm để ghi dữ liệu vào queue
		 * @param counter: Bộ đếm số phần tử hiện có trong queue
		 * @param end: Vị trí kết thúc của queue
		 */
		typedef struct {
			uint32_t	base; 		// Địa chỉ cơ sở của queue, nơi lưu trữ dữ liệu
			uint32_t	head; 		// Vị trí đầu của queue
			uint32_t	tail;   	// Vị trí cuối của queue
			uint32_t	q_len;		// Độ dài tối đa của queue
			uint32_t	e_len; 	 	// Độ dài của mỗi phần tử trong queue
			q_ctrl		read;   	// Con trỏ hàm để đọc dữ liệu từ queue
			q_ctrl		write;  	// Con trỏ hàm để ghi dữ liệu vào queue
			uint32_t	counter; 	// Bộ đếm số phần tử hiện có trong queue
			uint32_t	end; 			// Vị trí kết thúc của queue, được tính bằng base + (q_len * e_len)
		} queue_t;

		/**
		 * @brief Khai báo các hàm quản lý log queue
		 * @param q: Con trỏ đến cấu trúc log queue cần quản lý
		 * @param d: Con trỏ đến dữ liệu cần thêm vào hoặc lấy ra khỏi queue
		 * @return uint8_t: Trả về trạng thái của thao tác thêm hoặc lấy dữ liệu từ queue
		 */
		uint8_t		queue_init(
			queue_t* q, // Con trỏ đến cấu trúc log queue cần khởi tạo
			uint32_t b, 		// Địa chỉ cơ sở của queue, nơi lưu trữ dữ liệu
			uint32_t q_l, 	// Độ dài tối đa của queue, xác định số phần tử có thể chứa trong queue
			uint32_t e_l, 	// Độ dài của mỗi phần tử trong queue, xác định kích thước của dữ liệu được lưu trữ
			q_ctrl f_w, 		// Con trỏ hàm để ghi dữ liệu vào queue, được sử dụng để thêm phần tử vào queue
			q_ctrl f_r			// Con trỏ hàm để đọc dữ liệu từ queue, được sử dụng để lấy phần tử ra khỏi queue
		);

		/**
		 * @brief Khai báo các hàm add element vào log queue
		 * @param q: Con trỏ đến cấu trúc log queue cần quản lý
		 * @param d: Con trỏ đến dữ liệu cần thêm vào hoặc lấy ra khỏi queue
		 * @return uint8_t: Trả về trạng thái của thao tác thêm hoặc lấy
		 */
		uint8_t		queue_put(queue_t* q, void* d);

		/**
		 * @brief Khai báo các hàm get element từ log queue
		 * @param q: Con trỏ đến cấu trúc log queue cần quản lý
		 * @param d: Con trỏ đến dữ liệu cần thêm vào hoặc lấy ra khỏi queue
		 * @return uint8_t: Trả về trạng thái của thao tác thêm hoặc lấy
		 */
		uint8_t		queue_get(queue_t* q, void* d);

		/**
		 * @brief Khai báo hàm trả về số lượng phần tử hiện có trong log queue
		 * @param q: Con trỏ đến cấu trúc log queue cần quản lý
		 * @return uint32_t: Trả về số lượng phần tử hiện có trong queue
		 */
		uint32_t	queue_len(queue_t* q);

	#ifdef __cplusplus
	}
	#endif

#endif //__QUEUE_H__
