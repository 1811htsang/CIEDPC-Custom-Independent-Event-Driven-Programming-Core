# To-do list

Đây là nơi lưu trữ các công việc cần làm liên quan đến dự án CIEDPC cũng như các ghi chú và ý tưởng phát triển trong tương lai.

## Ghi chú phát triển

- Bổ sung lộ trình phát triển để đưa CIEDPC từ core EDP thành 1 OS hoàn chỉnh theo tiêu chuẩn Semantic Versioning.
- Bổ sung các đối chiếu về đánh giá thiết kế của CIEDPC với mô hình chuẩn là QP/C của Miro Samek.
- Bổ sung tài liệu để mention quá trình chuyển đổi thiết kế từ dependent của EPCB-vn AKEDP sang CIEDPC
- Quy ước: thay đổi tài liệu có thể được phát hành như patch version nếu ảnh hưởng trực tiếp đến release deliverable hoặc cách sử dụng hệ thống.

## Công việc cần làm

### Phiên bản 1.0.0

- [x] Hoàn thiện phân tích lõi tham chiếu AKEDP hiện có để đưa ra tài liệu phân tích & đề xuất cải tiến.
- [x] Remove các obsolete code và thay thế bằng cấu trúc thư mục + cấu trúc code mới.
- [x] Hoàn thiện thông tin core gốc
- [x] Hoàn thiện message pool implementation và tích hợp vào lõi CIEDPC.
- [x] Hoàn thiện FSM, TSM làm cơ sở để implement vào task driver.
- [x] Hoàn thiện task driver implementation và tích hợp vào lõi CIEDPC.
- [x] Hoàn thiện timer driver implementation và tích hợp vào lõi CIEDPC.
- [x] Hoàn thiện ISR Bridge implementation và tích hợp vào lõi CIEDPC.
- [x] Thiết kế testing chức năng trên task, timer, isr, message pool, tsm.
- [x] Bổ sung thiết kế testing MSG allocation cho task.
- [x] Bổ sung thiết kế testing FSM cho task.
- [x] Sửa đổi bổ sung thiết kế hàm khởi tạo FSM để tránh cyclic dependency giữa FSM, message pool và task driver.
- [x] Bổ sung API cho tương tác với task poll.
- [x] Bổ sung driver memrp để thực hiện memory profiling và tối ưu hóa memory footprint của lõi CIEDPC.
- [x] Viết tài liệu hướng dẫn sử dụng và phát triển lõi CIEDPC.
- [x] Kiểm tra các data types sử dụng nhằm thu gọn memory footprint.
- [x] Bổ sung linting và code formatting để đảm bảo codebase sạch sẽ và dễ đọc.

### Phiên bản 1.0.1

- [x] Bổ sung template code cho phần app layer để làm ví dụ cho việc phát triển ứng dụng trên nền tảng CIEDPC.
- [x] Import thiết kế vào STM32 nhằm thử nghiệm thực tế trên phần cứng.
- [x] Bổ sung RAM profiling cho memrp để đánh giá hiệu quả sử dụng bộ nhớ của lõi CIEDPC trên đa nền tảng.
- [x] Import các template code vào lại source code để ra mắt phiên bản 1.0.1 của lõi CIEDPC.
- [ ] Bổ sung tài liệu để hướng dẫn phát triển với nền tảng CIEDPC trên STM32, bao gồm hướng dẫn cài đặt môi trường phát triển, cấu hình phần cứng và ví dụ code.
- [ ] Bổ sung các tài liệu phân tích và thiết kế chi tiết cho các module của lõi CIEDPC, bao gồm FSM, TSM, task driver, timer driver và ISR bridge đã thất lạc vào thời điểm hoàn thiện phiên bản 1.0.0.
- [ ] Ra mắt phiên bản 1.0.1 của lõi CIEDPC với đầy đủ tài liệu hướng dẫn sử dụng và phát triển trên STM32.

### Phiên bản 1.0.2

- [ ] Hoàn thiện test case để thống nhất định dạng và quy trình testing cho các module của lõi CIEDPC.
- [ ] Bổ sung thiết kế 1 internal console để thay thế printf debugging trong để hỗ trợ kit không có cổng UART.
- [ ] Thiết kế getter để truyền dữ liệu từ internal console ra ngoài màn hình hoặc UART.
- [ ] Ra mắt phiên bản 1.0.2 của lõi CIEDPC với đầy đủ test case và tài liệu hướng dẫn sử dụng internal console.

### Phiên bản 1.1.x

- [ ] Bổ sung tài liệu đối chiếu thiết kế của CIEDPC với mô hình chuẩn QP/C của Miro Samek để làm rõ các điểm tương đồng và khác biệt trong kiến trúc và cách tiếp cận.
- [ ] Triển khai itnlog (Internal Logger) để lưu log nội bộ trên RAM/Flash và hỗ trợ cơ chế ghi đè vòng tròn - ring buffer đã có sẵn trong `common/containter/ring_buffer.h`.
- [ ] Tích hợp ciedpc_spy (Software Tracing) vào Scheduler và Message Pool để xuất trace phục vụ phân tích luồng xử lý.

### Phiên bản 1.2.x

- [ ] Thiết kế và triển khai Publish-Subscribe (Pub/Sub) engine để một sự kiện có thể phát tới nhiều task đã đăng ký.
- [ ] Hỗ trợ Multicasting để gửi một tin nhắn đến một nhóm ID cụ thể dựa trên ref_count của Message.
- [ ] Thiết kế Zero-copy Bridge để chuyển quyền sở hữu vùng nhớ giữa các pool mà không cần sao chép dữ liệu lớn.

### Phiên bản 1.3.x

- [ ] Tích hợp Tickless Idle Mode vào PAL để tạm dừng tick hệ thống khi CPU ở trạng thái idle và không còn timer chờ xử lý.
- [ ] Bổ sung Dynamic Priority Tuning để thay đổi độ ưu tiên của task tại runtime khi xảy ra tình huống khẩn cấp.
- [ ] Xây dựng CPU Load Monitor để tính toán % tải CPU dựa trên thời gian thực thi task và idle time.

### Phiên bản 1.4.x

- [ ] Tích hợp Kconfig system với kconfiglib để quản lý cấu hình Core, Pool size và TSM settings qua menuconfig.
- [ ] Thiết kế Virtual File System (VFS) layer để task có thể thao tác với Flash, EEPROM hoặc RAM như file.
- [ ] Xây dựng Component Manager để đóng gói các driver và module thành các CIEDPC Components tái sử dụng được.

### Phiên bản 1.5.x

- [ ] Hoàn thiện Task Guard với cơ chế heartbeat định kỳ để phát hiện task bị treo và kích hoạt safe response.
- [ ] Bổ sung Secure Signal Bridge với kiểm tra CRC và range check cho dữ liệu đi vào từ task_post_isr.
- [ ] Thiết kế Memory Protection Logic để giả lập phân vùng bộ nhớ và tận dụng MPU nếu phần cứng hỗ trợ.

### Lộ trình dự kiến để đưa CIEDPC từ core EDP thành 1 OS hoàn chỉnh

- [ ] Device Tree Support để cấu hình phần cứng qua file mô tả thay vì sửa code PAL.
- [ ] Kernel/User Space Separation để tách biệt hoàn toàn vùng nhớ của Core và App khi chip có MPU/MMU.
- [ ] Shell CLI để tương tác với OS qua terminal.
