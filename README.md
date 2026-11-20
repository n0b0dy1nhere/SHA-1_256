# SHA-1 / SHA-256

Case test huật toán băm SHA-1 và SHA-256 được hiện thực bằng C, kèm
hai tập lệnh Python dùng thư viện `hashlib` để kiểm chứng.

Mỗi thuật toán có hai bản cài đặt song song — một bản C viết tay và một bản
Python chuẩn để so sánh kết quả


## Tệp trong file

| Tệp | Ngôn ngữ | Nội dung |
| --- | --- | --- |
| `sha1final.c` | C | Cài đặt thuật toán SHA-1 (đệm dữ liệu, mở rộng 80 từ, 4 hàm vòng) cùng giao diện dòng lệnh (CLI) đọc từ stdin. |
| `sha256final.c` | C | Cài đặt thuật toán SHA-256 hoàn chỉnh (đệm dữ liệu, mở rộng 64 từ, trạng thái 8 từ, mảng hằng số $K$) cùng CLI đọc từ stdin. |
| `hashlibsha1.py` | Python | Script tính mã băm SHA-1 bằng hashlib, dùng làm kết quả chuẩn để đối chiếu. |
| `hashlibsha256.py` | Python | Script tính mã băm SHA-256 bằng hashlib, dùng làm kết quả chuẩn để đối chiếu. |

## Tổng quan thuật toán

Cả hai bản cài đặt C đều tự xây dựng toàn bộ quy trình xử lý mà không phụ thuộc vào thư viện mật mã bên ngoài:

1. **Đệm dữ liệu (*Padding*):** Thêm byte `0x80` vào cuối đầu vào, chèn thêm các byte `0x00` để đạt bội số của 64 byte (chừa lại 8 byte cuối), sau đó nối chiều dài bit gốc dưới dạng số nguyên 64-bit *big-endian*.
2. **Chia khối (*Parsing*):** Phân tách dữ liệu thành các khối (*block*) có kích thước cố định 64 byte (512 bit).
3. **Lịch thông điệp (*Message Schedule*):** Mở rộng 16 từ 32-bit đầu tiên thành **80 từ** (SHA-1) hoặc **64 từ** (SHA-256) theo quy tắc dịch và xoay bit riêng.
4. **Nén (*Compression*):** Cập nhật các thanh ghi trạng thái nội bộ qua **80 vòng** (SHA-1) hoặc **64 vòng** (SHA-256).
5. **Xuất mã băm (*Digest*):** Chuyển đổi trạng thái cuối cùng thành chuỗi thập lục phân (*hex*) dài **20 byte** (SHA-1) hoặc **32 byte** (SHA-256).

### So sánh SHA-1 và SHA-256

| Tiêu chí | SHA-1 | SHA-256 |
| :--- | :--- | :--- |
| **Độ dài digest** | 20 byte (160 bit) | 32 byte (256 bit) |
| **Số vòng nén** | 80 vòng | 64 vòng |
| **Từ trạng thái** | 5 × 32-bit | 8 × 32-bit |
| **Lịch thông điệp** | Phép `XOR` và xoay trái 1 bit | Các hàm `σ0`, `σ1` và phép cộng |
| **Hàm vòng & Hằng số** | Hàm `f` theo 4 giai đoạn, 4 hằng `K` | Hàm `Ch`, `Maj` và mảng 64 hằng `K` |

## Yêu cầu môi trường

* **Trình biên dịch C:** Hỗ trợ chuẩn **C11** trở lên (GCC, Clang hoặc MSVC). Mã nguồn hoàn toàn độc lập, chỉ sử dụng các thư viện chuẩn: `stdint.h`, `stdlib.h`, `string.h`, `stdio.h`.
* **Python:** **Python 3** (chỉ sử dụng thư viện tích hợp sẵn `hashlib`).

## Hướng dẫn biên dịch & sử dụng

### 1. Chương trình C

Biên dịch từng tệp mã nguồn thành tệp thực thi bằng GCC (hoặc trình biên dịch C11 tương đương):

```bash
# Biên dịch và chạy SHA-1
gcc -O2 -o sha1 sha1final.c
./sha1        # Trên Windows: .\sha1.exe

# Biên dịch và chạy SHA-256
gcc -O2 -o sha256 sha256final.c
./sha256      # Trên Windows: .\sha256.exe
```

Sau khi khởi chạy, chương trình sẽ nhắc nhập chuỗi đầu vào (tối đa **4096 byte**) từ `stdin` và in ra mã băm tương ứng:

```text
Enter input: hello world
SHA-1 digest: 2aae6c35c94fcfb415dbe95f408b9ce91ee846ed
```

### 2. Tập lệnh đối chứng Python

Chạy trực tiếp hai tệp Python để lấy kết quả tham chiếu từ `hashlib`:

```bash
python hashlibsha1.py      # -> SHA-1:  2aae6c35c94fcfb415dbe95f408b9ce91ee846ed
python hashlibsha256.py    # -> SHA-256: b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
```

## Kiểm chứng (Test Vectors)

Kết quả đầu ra từ hai bản cài đặt C khớp hoàn toàn với thư viện `hashlib` của Python cũng như các vector kiểm thử tiêu chuẩn từ NIST (**FIPS 180-1** và **FIPS 180-4**):

| Input | SHA-1 | SHA-256 |
| :--- | :--- | :--- |
| `"abc"` | `a9993e36…23e1adc6` | `ba7816bf…f41dd077` |
| `""` *(chuỗi rỗng)* | `da39a3ee…b0bbe32d` | `e3b0c442…855a0d00` |

> **Lưu ý:** Ký hiệu `…` thay thế cho phần ký tự ở giữa của chuỗi hex. Chạy trực tiếp chương trình để xem đầy đủ toàn bộ mã băm.
