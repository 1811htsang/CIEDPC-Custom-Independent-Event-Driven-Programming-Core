#ifndef __PORT_H__
  #define __PORT_H__

  // Khai báo các attribute cho AK
  #define __AK_PACKETED	__attribute__((__packed__)) // Cho phép định nghĩa struct gọn nhất
  #define __AK_WEAK		  __attribute__((__weak__))   // Hỗ trợ overriding

  // Khai báo kích thước của control block cho malloc
  #define __AK_MALLOC_CTRL_SIZE	8

  // Khai báo hỗ trợ tuân thủ
  #ifndef PROGMEM
    #define PROGMEM
  #endif

#endif //__PORT_H__
