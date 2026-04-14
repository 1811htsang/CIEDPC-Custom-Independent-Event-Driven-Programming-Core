#ifndef __UTILS_H__
  #define __UTILS_H__

  // Khai báo directive C cho C++
  #ifdef __cplusplus
    extern "C"
  {
  #endif

    // Khai báo các thư viện sử dụng
    #include <stdint.h>
    #include <string.h>

    // Khai báo hàm xử lý bộ nhớ
    extern void* mem_set(void *str, int c,size_t size);
    extern void* mem_cpy(void *dst,const void *str, size_t size);
    extern int mem_cmp(const void * ptr1, const void * ptr2, size_t num);
    extern uint8_t mem_read(uint32_t, uint8_t*, uint32_t);
    extern uint8_t mem_write(uint32_t, uint8_t*, uint32_t);

    // Khai báo hàm xử lý chuỗi ký tự
    extern uint32_t str_len(const int8_t *str);
    extern int32_t str_cmp(const int8_t *str1, const int8_t *str2);
    extern int8_t * str_cpy (int8_t * destination, const int8_t * source);

  #ifdef __cplusplus
    }
  #endif

#endif //__UTILS_H__
