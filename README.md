# SHA Visualization

Ứng dụng desktop trên Windows hỗ trợ kiểm tra tính toàn vẹn của tệp tin và trực quan hóa cách các thuật toán băm **SHA-1** và **SHA-256** hoạt động bên dưới lớp vỏ.

Dự án kết hợp hiệu năng xử lý của lõi tính toán viết bằng **C** (`shaNative.dll`) với giao diện hiện đại trên nền tảng **WinUI 3**. Các tác vụ duyệt thư mục, tạo manifest, quản lý lịch sử và cấu hình được đảm nhiệm bởi tầng mã nguồn managed (.NET 8). Ứng dụng đặt hai thuật toán song song để làm nổi bật sự khác biệt cốt lõi (80 vòng lặp so với 64 vòng lặp, quy tắc mở rộng lịch thông điệp), đồng thời cung cấp chế độ **Learn** mô phỏng chi tiết dữ liệu đi qua toàn bộ 7 giai đoạn xử lý.


## Tính năng chính

| Trang | Chức năng |
| :--- | :--- |
| **Home** | Kiểm tra trạng thái backend native (trạng thái nạp DLL, độ tương thích phiên bản ABI) và cung cấp các lối tắt thao tác nhanh. |
| **Verify** | Tính mã băm cho văn bản, tệp đơn lẻ hoặc toàn bộ thư mục; đối chiếu với digest kỳ vọng (khớp / không khớp); hỗ trợ hủy tác vụ chạy dài và xem manifest thư mục. |
| **Learn** | Quan sát từng khối dữ liệu đi qua 7 giai đoạn: *Đầu vào → Đệm (Padding) → Chia khối → Lịch thông điệp → Nén → Cập nhật trạng thái → Digest*, kèm trạng thái thanh ghi tại từng vòng. |
| **Collision Demo** | Minh họa thực tế qua hai tệp PDF từ nghiên cứu *SHAttered*: khác nhau về dữ liệu byte nhưng cho ra cùng mã SHA-1 (`38762cf7…`), trong khi SHA-256 hoàn toàn khác biệt. *(Ứng dụng chỉ băm các tệp mẫu đóng gói sẵn, không tạo va chạm mới).* |
| **History** | Ghi lại toàn bộ lịch sử chạy vào cơ sở dữ liệu SQLite cục bộ kèm thời gian thực thi, thông lượng xử lý và biểu đồ trực quan hóa thông lượng. |
| **Settings** | Tùy chỉnh thuật toán mặc định, kích thước bộ đệm (*buffer size*), chính sách xử lý tệp ẩn / *reparse point* và bật/tắt lưu lịch sử (lưu trữ chung trong SQLite). |

## Cấu trúc dự án

```text
.
├── native/                 # Backend C: sha1.c, sha256.c, streaming, file hashing, trace
│   ├── include/            # shagui_native.h — định nghĩa ABI cố định (SHAGUI_ABI_VERSION 1)
│   ├── src/                # Mã nguồn cài đặt lõi và sha_internal.h
│   └── tests/              # Bộ kiểm thử native (vectors, streaming, file, trace)
├── src/
│   ├── ShaGui.Core/        # Lõi managed: P/Invoke, băm dữ liệu, manifest, lịch sử SQLite, cài đặt
│   └── ShaGui.App/         # Giao diện WinUI 3 và 6 trang chức năng
└── docs/                   # Tài liệu kỹ thuật: NATIVE_API.md, MANIFEST.md
```

## Yêu cầu hệ thống

* **Hệ điều hành:** Windows 10 phiên bản 1809 trở lên (x64) — dự án nhắm mục tiêu `net8.0-windows10.0.19041.0` (tối thiểu `10.0.17763.0`).
* **SDK:** [.NET 8 SDK](https://dotnet.microsoft.com/download/dotnet/8.0).
* **Trình biên dịch C (để build DLL native):** **MSVC** (Visual Studio Build Tools — bộ công cụ chính thức) hoặc **GCC** thông qua MSYS2 (lựa chọn thay thế).
* **Công cụ chạy & kiểm thử UI:** [WinApp CLI](https://learn.microsoft.com/windows/apps/) (`winget install Microsoft.WinApp`).

## Hướng dẫn Build

Quá trình build gồm **2 bước tuần tự** — cần biên dịch thư viện native trước, bởi `ShaGui.Core` sẽ báo lỗi ngay nếu không tìm thấy `shaNative.dll`:

```powershell
# Bước 1: Biên dịch backend native (shaNative.dll)
# Mở "x64 Native Tools Command Prompt for VS 2022" và chạy từ thư mục gốc của dự án:
New-Item -ItemType Directory -Force native\build\Release\msvc | Out-Null
Push-Location native\build\Release\msvc
cl /nologo /W4 /std:c17 /utf-8 /D_CRT_SECURE_NO_WARNINGS /O2 /DSHAGUI_BUILD /LD `
   /I..\..\..\include /I..\..\..\src `
   ..\..\..\src\sha1.c ..\..\..\src\sha256.c ..\..\..\src\sha_api.c `
   ..\..\..\src\sha_ctx.c ..\..\..\src\sha_file.c ..\..\..\src\sha_trace.c `
   /Fe:shaNative.dll /link /IMPLIB:shaNative.lib
Pop-Location

# Bước 2: Build solution managed (Core + App)
dotnet build ShaGui.sln -c Debug
```

**Lưu ý khi build:**
* Tệp DLL bắt buộc phải nằm đúng tại đường dẫn `native\build\Release\msvc\shaNative.dll` để `ShaGui.Core.csproj` có thể tham chiếu.
* Tệp `Directory.Build.props` cố định toàn bộ các project C# ở kiến trúc **x64** nhằm đồng bộ với `shaNative.dll` (tránh lỗi `BadImageFormatException` nếu test host tự chạy dưới tiến trình 32-bit).
* Để chạy bộ kiểm thử native, hãy biên dịch tương tự Bước 1 nhưng thay cờ `/LD` bằng `/DSHAGUI_STATIC` và trỏ tới các tệp trong `native\tests\*.c`.

## Chạy ứng dụng

Khởi chạy ứng dụng trực tiếp từ dòng lệnh bằng WinApp CLI:

```powershell
winapp run . --project ShaGui.App
```

*(Chế độ chạy đóng gói yêu cầu bật **Developer Mode** trên Windows hoặc cài đặt chứng chỉ số đã được tin cậy).*

## Đóng gói (MSIX)

```powershell
# 1. Tạo chứng chỉ phát triển khớp với Publisher trong Package.appxmanifest (chỉ cần làm một lần)
winapp cert generate --manifest .\src\ShaGui.App\Package.appxmanifest

# 2. Đóng gói bản Release x64 ra tệp MSIX và ký bằng chứng chỉ vừa tạo
winapp package .\src\ShaGui.App\ShaGui.App.csproj --arch x64 --cert .\devcert.pfx
```

* Tệp `devcert.pfx` chỉ phục vụ mục đích phát triển (mật khẩu mặc định: `password`), đã được loại trừ trong `.gitignore` và tuyệt đối không phát hành công khai.
* Ứng dụng chỉ hỗ trợ kiến trúc **x64** do phụ thuộc vào thư viện native x64. Khi cài đặt tệp `.msix` tải về từ mục *GitHub Releases*, người dùng cần bật **Developer Mode** hoặc cài đặt chứng chỉ ký tương ứng vào máy.

## Tài liệu kỹ thuật

* [ARCHITECTURE.md](ARCHITECTURE.md) — Kiến trúc phân lớp, luồng dữ liệu, xử lý đa luồng và quy trình đóng gói.
* [docs/NATIVE_API.md](docs/NATIVE_API.md) — Đặc tả giao tiếp `shaNative.dll`: danh sách 18 hàm export, cấu trúc dữ liệu, mã trạng thái và lưu ý P/Invoke.
* [docs/MANIFEST.md](docs/MANIFEST.md) — Quy chuẩn định dạng văn bản cho manifest thư mục (bảng mã, thứ tự sắp xếp, chính sách duyệt tệp).

## Contribute

* **Tệp minh họa va chạm SHAttered:** Nghiên cứu của Marc Stevens (CWI Amsterdam), Elie Bursztein (Google), Pierre Karpman, Ange Albertini, Yarik Markov, Alex Petit Bianco và Clement Baisse — nguồn từ [shattered.io](https://shattered.io), được tích hợp phục vụ mục đích giáo dục.
* **Thuật toán băm:** Phát triển dựa trên đặc tả chuẩn của SHA-1 / SHA-256 và đối chiếu kiểm chứng với thư viện `hashlib` (Python) cùng `System.Security.Cryptography` (.NET).
