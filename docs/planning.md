# Kế hoạch phát triển CIEDPC

Bản kế hoạch này sẽ định hướng chi tiết cho việc phát triển CIEDPC, tập trung vào việc giải phóng lõi khỏi phần cứng cụ thể, xây dựng hệ thống tin nhắn mới, và tối ưu hóa Task State Machine (TSM) theo chuẩn ICONS 2011. Mục tiêu cuối cùng là tạo ra một thư viện Scheduler hoàn chỉnh, có thể chạy trên nhiều nền tảng khác nhau mà không cần sửa đổi lõi.

---

## I. Kiến trúc Phân tầng Tổng thể (Architecture)

CIEDPC sẽ hoạt động dựa trên mô hình 4 tầng tách biệt hoàn toàn:

1. **CIEDPC Core (Hạt nhân):** Logic thuần túy (C-Pure). Chỉ chứa thuật toán Scheduler, Message Queue, Timer, FSM/TSM. Không được phép chứa bất kỳ từ khóa `#include <stm32...>` hay lệnh ASM nào.
2. **PAL (Platform Abstraction Layer):** Tầng trung gian. Core sẽ định nghĩa "Nhu cầu" (Prototype), và PAL sẽ cung cấp "Thực thi" (Implementation).
3. **App Layer (Ứng dụng):** Chứa logic Task và FSM của người dùng.
4. **Interface Layer (Cổng ngoài):** `task_if` đóng vai trò Gateway xử lý serialization/deserialization tín hiệu từ thế giới bên ngoài.

---

## II. Logic Thiết kế Chi tiết (Design Logic)

### 1. Cơ chế PAL (Dependency Inversion)

Thay vì Core chủ động gọi Driver, Core sẽ yêu cầu các "Dịch vụ hệ thống".

* **Critical Section:** Dùng `pal_enter_critical()`/`pal_exit_critical()`. Trên STM32 nó là tắt ngắt, trên Linux nó là Mutex.
* **Bit Manipulation:** Thay thế `__CLZ` bằng `pal_get_highest_priority()`.

### 2. Hệ thống Tín hiệu & Tin nhắn Hợp nhất (Unified Signal/Message)

* **ISR-Safe Injection:** Bổ sung một **Internal Ring Buffer** (lock-free) nằm giữa PAL và Core. Khi có ngắt (UART/Timer), PAL đẩy Signal vào Ring Buffer này. Core sẽ "drain" (rút dữ liệu) từ Ring Buffer này vào các Task Queue ở đầu mỗi chu kỳ Scheduler. Điều này loại bỏ hoàn toàn việc Core phải biết về ISR.
* **Message Pooling:** Hợp nhất 3 loại thành 1 API: `ciedpc_msg_alloc(size)`. Core tự động điều phối từ Pure Pool (size=0), Common Pool (size nhỏ) hoặc Heap (size lớn) thông qua cấu hình PAL.

### 3. Task State Machine (TSM) & FSM (Theo chuẩn ICONS 2011)

* Tận dụng bảng băm hoặc mảng 2 chiều để tối ưu hóa việc Dispatch sự kiện (đạt mục tiêu ~18 instruction cycles như tài liệu nghiên cứu).
* Hỗ trợ `SIG_TSM_ENTRY` và `SIG_TSM_EXIT` mặc định trong Core để quản lý vòng đời Task.

---

## III. Sơ đồ Cấu trúc Thư mục CIEDPC

```text
CIEDPC/
├── core/                        # LÕI CHÍNH (Bất biến)
│   ├── inc/                     # ciedpc_msg.h, ciedpc_task.h, ciedpc_timer.h, ciedpc_fsm.h, ciedpc_tsm.h
│   └── src/                     # Logic scheduler, timer engine, message manager
├── pal/                         # BACKEND (Lớp trừu tượng)
│   ├── ciedpc_pal.h             # Khai báo thống nhất chung cho toàn bộ PAL
│   ├── services/                # Hardware Services (Mapping phần cứng)
│   │   ├── timer/               # pal_timer_hw.c/h
│   │   ├── reset/               # pal_reset_hw.c/h
│   │   ├── info/                # pal_version_info.c/h
│   │   ├── memrp/               # pal_memory_report.c/h + linker_templates/
│   │   ├── debug/               # pal_console_io.c/h
│   │   └── fatal/               # pal_panic_handler.c/h (Cấp cứu khi lỗi core)
│   ├── core_support/            # Logic Services (Thuật toán hỗ trợ core)
│   │   ├── math/                # pal_bit_algo.c/h (log2lkup, de-bruijn)
│   │   └── conc/                # pal_sync.c/h (Critical section lồng nhau)
│   └── arch/                    # Implementation (Mã nguồn chi tiết từng chip)
│       ├── stm32f10x/           # porting_stm32f1.c
│       ├── esp32/               # porting_esp32.c
│       └── posix/               # porting_linux.c (để simulation)
├── app/                         # FRONTEND (Tầng ứng dụng)
│   ├── config/                  # app_config.h, ciedpc_user_config.h
│   ├── tasks/                   # task_player.cpp, task_game_logic.cpp (Tách nhỏ thay vì 1 file app.cpp)
│   ├── declaration/             # app_signals.h, app_types.h
│   └── interface/               # task_if.cpp (Gateway chuyển đổi external event)
├── common/                      # UTILS (Cấu trúc dữ liệu thuần C)
│   ├── container/               # fifo.c/h, ring_buffer.c/h, linked_list.c/h
│   └── math_utils/              # Các hàm toán học bổ trợ khác
└── build/                       # BUILD SYSTEM
    ├── cmake/                   # ciedpc_compiler_flags.cmake
    └── Makefile                 # Build script hỗ trợ chọn PLATFORM=...
```

---

## IV. Kế hoạch Triển khai Chi tiết (Implementation Roadmap)

### Giai đoạn 1: Giải phóng Core (Hardware Independence)

* **Bước 1:** Tạo `ciedpc_pal.h`. Chuyển toàn bộ các macro như `ENTRY_CRITICAL`, `LOG2LKUP`, `GET_TICK` thành các hàm `pal_xxx`.
* **Bước 2:** Xóa bỏ tất cả các include liên quan đến STM32 trong thư mục `core/`.
* **Bước 3:** Chỉnh sửa `task_ready` từ `uint8_t` sang `uint32_t` để hỗ trợ tối đa 32 mức ưu tiên (Mở rộng khả năng scale).
* **Bước 4:** Fix bug self-include trong `timer.h`.

### Giai đoạn 2: Xây dựng Hệ thống Tin nhắn & Signal mới

* **Bước 5:** Triển khai **Unified Message API**. Viết logic tự động chọn Pool dựa trên tham số `size`.
* **Bước 6:** Xây dựng **Signal Injection Buffer**. Đây là một Ring Buffer nằm trong Core nhưng được PAL gọi thông qua hàm `ciedpc_rx_signal_from_isr()`.
* **Bước 7:** Hoàn thiện `task_if` Gateway. Thêm hàm `serialize/deserialize` để đóng gói tin nhắn nhị phân theo frame truyền thông.

### Giai đoạn 3: Nâng cấp TSM & FSM chuyên sâu

* **Bước 8:** Cấu trúc lại `tsm.c` để hỗ trợ cơ chế Entry/Exit Action.
* **Bước 9:** Áp dụng mô hình Dispatcher từ tài liệu ICONS 2011 vào `fsm.c` để tăng tốc độ phản ứng sự kiện.

### Giai đoạn 4: Đa nền tảng & Build System

* **Bước 10:** Viết `CMakeLists.txt` hoặc refactor `Makefile`. Sử dụng biến môi trường `PLATFORM` để link đúng thư mục trong `pal/arch/`.
* **Bước 11:** Tạo bản Porting mẫu trên **Linux (POSIX)**. Đây là bước quan trọng nhất để chứng minh Core hoàn toàn độc lập với vi điều khiển.

---

## V. Logic "Import không cần Rewrite"

Để đạt được mục tiêu này, CIEDPC sẽ áp dụng cơ chế **Cấu hình thông qua Header App**:

1. Khi bạn muốn dùng CIEDPC, bạn copy thư mục `core/` và `common/` vào dự án.
2. Bạn tự viết file `app_config.h` để định nghĩa số lượng Task.
3. Bạn chọn file Port tương ứng trong `pal/arch/`.
4. **Kết quả:** Bạn không cần mở bất kỳ file nào trong `core/` để sửa đổi. Lõi Core hoạt động như một thư viện đóng gói hoàn chỉnh.

---

## VI. Đánh giá Thiết kế CIEDPC

* **Tính linh hoạt:** Chạy được từ chip 8-bit (AVR) đến 32-bit (ARM/ESP32) và cả PC.
* **Tính an toàn:** Cơ chế Injection từ ISR thông qua Ring Buffer giúp Core không bao giờ bị rơi vào tình trạng Race Condition khi xử lý ngắt.
* **Tính tối ưu:** Hợp nhất Message giúp người dùng chỉ cần học 1 API duy nhất, giảm thiểu lỗi "nhầm loại message".

**Kế hoạch này đã sẵn sàng để triển khai.** Bạn có muốn bắt đầu bằng việc xây dựng file **`ciedpc_pal.h`** - "bản hiến pháp" để tách biệt Core khỏi phần cứng không?
