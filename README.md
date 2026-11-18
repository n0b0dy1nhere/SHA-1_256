# SHA-1 / SHA-256

Thuật toán băm SHA-1 và SHA-256 được hiện thực **bằng C**, kèm
hai tập lệnh Python dùng thư viện `hashlib` để kiểm chứng. File được dùng
để phân tích cách 2 thuật toán băm hoạt động

Mỗi thuật toán có hai bản cài đặt song song — một bản C viết tay và một bản
Python chuẩn làm "đối chứng" — để so sánh kết quả và hiểu rõ từng giai đoạn
của phép tính.

> **Lưu ý phạm vi:** SHA-1 tồn tại ở đây cho mục đích giáo dục và kiểm tra
> tương thích cũ. Nó không chống va chạm. Hãy dùng SHA-256 khi digest phải
> được tin tưởng.

## Tệp trong kho

| Tệp | Ngôn ngữ | Nội dung |
| --- | --- | --- |
| `sha1final.c` | C | SHA-1 viết tay: lịch tin nhắn 80 từ, 4 hàm vòng, đệm chuẩn, CLI đọc từ stdin. |
| `sha256final.c` | C | SHA-256 viết tay: lịch tin nhắn 64 từ + mảng hằng `K`, trạng thái 8 từ, đệm chuẩn, CLI đọc từ stdin. |
| `hashlibsha1.py` | Python | SHA-1 dùng `hashlib` — kết quả đối chứng. |
| `hashlibsha256.py` | Python | SHA-256 dùng `hashlib` — kết quả đối chứng. |

## Thuật toán

Cả hai bản C đều tự hiện thực toàn bộ pipeline — không gọi thư viện băm có
sẵn:

1. **Đệm** — thêm bit `0x80`, phần đệm về bội số của 64 byte, rồi nối chiều
   dài bit dạng big-endian 8 byte.
2. **Chia khối** — dữ liệu được xử lý theo từng khối 64 byte.
3. **Lịch tin nhắn** — mở rộng 16 từ đầu thành 80 từ (SHA-1) hoặc 64 từ
   (SHA-256) theo quy tắc riêng.
4. **Nén** — cập nhật trạng thái qua 80 vòng (SHA-1) hoặc 64 vòng (SHA-256)
   với các hằng số và hàm vòng khác nhau.
5. **Digest** — xuất trạng thái cuối dưới dạng hex: 20 byte (SHA-1) hoặc
   32 byte (SHA-256).

Điểm khác biệt chính giữa hai thuật toán được đặt cạnh nhau:

| | SHA-1 | SHA-256 |
| --- | --- | --- |
| Độ dài digest | 20 byte | 32 byte |
| Vòng nén | 80 | 64 |
| Từ trạng thái | 5 × 32-bit | 8 × 32-bit |
| Lịch tin nhắn | XOR + xoay trái 1 bit | `σ0` / `σ1` + cộng |
| Hàm vòng | `f` theo 4 giai đoạn, hằng `K` | `Ch` / `Maj`, hằng `K` 64 giá trị |

## Yêu cầu

- **C**: trình biên dịch C chuẩn C11 (như MSVC hoặc GCC). Cả hai tệp đều
  tự chứa, chỉ dùng các header chuẩn (`stdint.h`, `stdlib.h`, `string.h`,
  `stdio.h`).
- **Python**: Python 3 — chỉ dùng thư viện chuẩn `hashlib`.

## Build & chạy

Bản C biên dịch thành một file thực thi có CLI. Ví dụ với GCC:

```powershell
# SHA-1
gcc -O2 -o sha1 sha1final.c
.\sha1.exe

# SHA-256
gcc -O2 -o sha256 sha256final.c
.\sha256.exe
```

Chương trình nhắc nhập một chuỗi (tối đa 4096 byte) rồi in digest hex tương
ứng:

```
Enter input: hello world
SHA-1 digest: 2aae6c35c94fcfb415dbe95f408b9ce91ee846ed
```

Bản Python chạy trực tiếp:

```powershell
python hashlibsha1.py      # -> SHA-1:  2aae6c35c94fcfb415dbe95f408b9ce91ee846ed
python hashlibsha256.py    # -> SHA-256: b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
```

## Kiểm chứng

Kết quả của bản C viết tay phải khớp tuyệt đối với bản Python `hashlib` và
với các vector kiểm thử chuẩn (FIPS 180-1 / 180-4). Ví dụ:

| Input | SHA-1 | SHA-256 |
| --- | --- | --- |
| `abc` | `a9993e36…23e1adc6` | `ba7816bf…f41dd077` |
| *rỗng* | `da39a3ee…b0bbe32d` | `e3b0c442…855a0d00` |

> Dấu `…` thay cho phần còn lại của digest — chạy chương trình để xem đầy đủ.

## Ghi công

- Tham chiếu SHA-1 và SHA-256 dẫn xuất từ các nguồn C gốc của hai thuật
  toán; được kiểm chứng chéo bằng Python `hashlib` và vector FIPS.
