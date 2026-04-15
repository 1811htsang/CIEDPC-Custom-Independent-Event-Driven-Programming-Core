// Khai báo thư viện sử dụng
#include <cstring>
#include "main.h"
#include "app.h"
#include "platform.h"
#include "sys_cfg.h"
#include "sys_ctrl.h"

// Khai báo directive C cho C++
extern "C" {
	// Biến toàn cục chứa thông tin hệ thống
	system_info_t system_info;

	// Biến toàn cục cho soft watchdog
	static uint32_t soft_watchdog_limit_ticks = 0; 		// Số tick giới hạn trước khi soft watchdog kích hoạt reset
	static uint32_t soft_watchdog_counter_ticks = 0;	// Bộ đếm tick hiện tại của soft watchdog
	static uint8_t soft_watchdog_enabled = 0;					// Cờ cho biết soft watchdog có đang được kích hoạt hay không
	extern uint32_t _etext;														// Biến toàn cục chứa địa chỉ kết thúc của phần text (code) trong flash
}

// Hàm nội bộ để kích hoạt bộ đếm chu kỳ DWT, cho phép đo thời gian chính xác
static void enable_dwt_cycle_counter(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0U;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// Hàm để nhập và thoát critical section, sử dụng __disable_irq và __enable_irq để quản lý ngắt
void platform_enter_critical(void)
{
	__disable_irq();
}

// Hàm để thoát critical section, sử dụng __enable_irq để cho phép ngắt trở lại
void platform_exit_critical(void)
{
	__enable_irq();
}

// Hàm để cấu hình clock của hệ thống, thiết lập nhóm ưu tiên ngắt và kích hoạt bộ đếm chu kỳ DWT để đo thời gian chính xác
void sys_cfg_clock(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	enable_dwt_cycle_counter();
	system_info.cpu_clock = HAL_RCC_GetHCLKFreq();
}

// Hàm để cấu hình tick của hệ thống, thiết lập ưu tiên ngắt cho SysTick để đảm bảo rằng tick có độ ưu tiên thấp nhất
void sys_cfg_tick(void)
{
	HAL_NVIC_SetPriority(SysTick_IRQn, 15U, 0U);
}

// Hàm để cập nhật thông tin hệ thống, tính toán các giá trị như flash đã sử dụng, RAM đã sử dụng, kích thước dữ liệu đã khởi tạo, kích thước dữ liệu chưa khởi tạo, bộ nhớ stack còn lại, bộ nhớ heap còn lại và các thông tin khác dựa trên các biểu tượng linker script
void sys_cfg_update_info(void)
{
	extern uint32_t _etext;
	extern uint32_t _sdata;
	extern uint32_t _edata;
	extern uint32_t _sbss;
	extern uint32_t _ebss;
	extern uint32_t _estack;
	extern uint32_t _end;

	system_info.flash_used = (uint32_t)&_etext - FLASH_BASE;
	system_info.ram_used = (uint32_t)&_estack - (uint32_t)&_sdata;
	system_info.data_init_size = (uint32_t)&_edata - (uint32_t)&_sdata;
	system_info.data_non_init_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	system_info.stack_avail = (uint32_t)&_estack - (uint32_t)&_ebss;
	system_info.heap_avail = (uint32_t)&_estack - (uint32_t)&_end;
	system_info.ram_other = 0U;
}

// Hàm để khởi tạo cấu hình hệ thống, gọi các hàm cấu hình clock, tick và cập nhật thông tin hệ thống
void sys_cfg_init(void)
{
	sys_cfg_clock();
	sys_cfg_tick();
	sys_cfg_update_info();
}

// Hàm để reset hệ thống, sử dụng NVIC_SystemReset để khởi động lại vi điều khiển
void sys_ctrl_reset(void)
{
	NVIC_SystemReset();
}

// Hàm để khởi tạo soft watchdog, thiết lập thời gian timeout và kích hoạt soft watchdog
void sys_ctrl_soft_watchdog_init(uint32_t timeout_ticks)
{
	soft_watchdog_limit_ticks = timeout_ticks;
	soft_watchdog_counter_ticks = 0U;
	soft_watchdog_enabled = 1U;

	if (timeout_ticks == 0) {
    soft_watchdog_enabled = 0U; // Vô hiệu hóa nếu timeout không hợp lệ
    return;
	}
}

// Hàm để reset soft watchdog, đặt lại bộ đếm tick của soft watchdog về 0
void sys_ctrl_soft_watchdog_reset(void)
{
	soft_watchdog_counter_ticks = 0U;
}

// Hàm để kích hoạt soft watchdog, đặt cờ enabled của soft watchdog thành 1 để cho phép soft watchdog hoạt động
void sys_ctrl_soft_watchdog_enable(void)
{
	soft_watchdog_enabled = 1U;
}

// Hàm để vô hiệu hóa soft watchdog, đặt cờ enabled của soft watchdog thành 0 để ngăn chặn soft watchdog hoạt động
void sys_ctrl_soft_watchdog_disable(void)
{
	soft_watchdog_enabled = 0U;
}

// Hàm để tăng bộ đếm của soft watchdog, được gọi định kỳ để cập nhật bộ đếm 
void sys_ctrl_soft_watchdog_increase_counter(void)
{
	if ((soft_watchdog_enabled != 0U) && (soft_watchdog_limit_ticks != 0U)) {
		soft_watchdog_counter_ticks++;
		if (soft_watchdog_counter_ticks >= soft_watchdog_limit_ticks) {
			sys_ctrl_reset();
		}
	}
}

// Hàm để tạo độ trễ bằng cách sử dụng vòng lặp đếm, sử dụng __NOP để đảm bảo rằng vòng lặp không bị tối ưu hóa hoàn toàn
void sys_ctrl_delay(volatile uint32_t count)
{
	while (count-- != 0U) {
		__NOP();
	}
}

// Hàm để tạo độ trễ bằng cách sử dụng HAL_Delay, cho phép tạo độ trễ chính xác dựa trên tick của hệ thống
void sys_ctrl_delay_ms(volatile uint32_t count)
{
	HAL_Delay(count);
}

// Hàm để tạo độ trễ bằng cách sử dụng bộ đếm chu kỳ DWT, cho phép tạo độ trễ chính xác dựa trên số chu kỳ của CPU
void sys_ctrl_delay_us(volatile uint32_t count)
{
	uint32_t start = DWT->CYCCNT;
	uint32_t cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;
	uint32_t target = count * cycles_per_us;
	while ((DWT->CYCCNT - start) < target) {
	}
}

// Hàm để lấy thời gian hiện tại tính bằng microsecond, sử dụng bộ đếm chu kỳ DWT để tính toán thời gian dựa trên số chu kỳ đã trôi qua
uint32_t sys_ctrl_micros(void)
{
	uint32_t cycles_per_us = HAL_RCC_GetHCLKFreq() / 1000000U;
	if (cycles_per_us == 0U) {
		return 0U;
	}
	return DWT->CYCCNT / cycles_per_us;
}

// Hàm để lấy thời gian hiện tại tính bằng millisecond, sử dụng HAL_GetTick để trả về số tick đã trôi qua kể từ khi hệ thống khởi động
uint32_t sys_ctrl_millis(void)
{
	return HAL_GetTick();
}

// Hàm để vào chế độ chờ, sử dụng lệnh __WFI để đưa vi điều khiển vào chế độ chờ cho đến khi có ngắt xảy ra
void sys_ctrl_sleep_wait_for_irq(void)
{
	__WFI();
}

// Hàm để lấy số ngoại lệ hiện tại, sử dụng hàm __get_IPSR để trả về số ngoại lệ đang được xử lý
uint32_t sys_ctrl_get_exception_number(void)
{
	return __get_IPSR();
}

// Hàm để khởi động lại ứng dụng, gọi hàm reset của hệ thống để khởi động lại vi điều khiển
void sys_ctrl_restart_app(void)
{
	sys_ctrl_reset();
}

// Hàm để dừng MCU, sử dụng HAL_PWR_EnterSTANDBYMode để đưa vi điều khiển vào chế độ standby, giảm thiểu tiêu thụ năng lượng
void sys_ctrl_stop_mcu(void)
{
	HAL_PWR_EnterSTANDBYMode();
}

// Hàm kiểm tra xem hệ thống có đang chạy trong bootloader hay không, dựa trên giá trị của biểu tượng linker script để xác định
int asm_test_add(int lhs, int rhs)
{
	return lhs + rhs;
}

// Hàm để lấy kích thước stack đã sử dụng, tính toán dựa trên địa chỉ của _estack và vị trí hiện tại của stack pointer
uint32_t sys_stack_count_init(void)
{
	return sys_stack_get_size();
}

// Hàm để lấy kích thước stack còn lại, tính toán dựa trên địa chỉ của _estack và vị trí hiện tại của stack pointer
uint32_t sys_stack_usage(void)
{
	extern uint32_t _estack;
	uint32_t current_sp = __get_MSP();
	return (uint32_t)&_estack - current_sp;
}

// Hàm để lấy tổng kích thước stack, tính toán dựa trên địa chỉ của _estack và _ebss để xác định kích thước stack có sẵn
uint32_t sys_stack_get_size(void)
{
	extern uint32_t _estack;
	extern uint32_t _ebss;
	return (uint32_t)&_estack - (uint32_t)&_ebss;
}

