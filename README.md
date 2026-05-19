# Block Puzzle Game

Game xếp hình block viết bằng C++ và SFML 2.5.

## Tính năng
- **Classic** — chơi đơn, tính điểm và kỷ lục
- **1v1 Đối kháng** — hai người luân phiên trên cùng máy, không so điểm; ai khiến đối thủ không đặt được nữa thì thắng
- **Mốc đối chiến** (menu) — Block chớp, Block nhanh, Theo giờ (1/3/5h), Theo ngày (1/3/7) *(giao diện sẵn)*
- 36 loại hình block (tối đa 3×3, dễ đến vừa — không có khối 4 ô thẳng)
- Tray 3 block — đặt xong 1 ô là có khối mới ngay tại ô đó
- Điểm theo số hàng/cột xóa cùng lúc (1→10, 2→40, 3→90, 4+→×4)
- Thưởng ×5 khi xóa sạch bảng sau lượt xóa
- High score lưu file `highscore.dat`, xếp hạng khi Game Over
- Màn hình Game Over hiển thị "NEW BEST!" nếu phá kỷ lục
- Hit-test chính xác theo pixel của từng ô block

## Build nhanh (Windows)

### Yêu cầu
- SFML 2.5 cài vào `C:\SFML` → https://www.sfml-dev.org/download.php
- TDM-GCC-64 (kèm với Dev-C++) hoặc MinGW-w64

### Các bước
```bat
build_windows.bat
run_windows.bat
```

Hoặc double-click `Play_Game.bat` (có console) / `Play_Game.vbs` (không console).

## Assets tùy chọn
Đặt vào thư mục `assets/`:
- `arial.ttf` — font chữ (nếu thiếu dùng font hệ thống)
- `place.wav`, `clear.wav`, `combo.wav`, `gameover.wav` — âm thanh

## Luật chơi
- Kéo block từ tray xuống bảng 8×8
- Khi một hàng hoặc cột đầy → tự xóa, cộng điểm
- Xóa 2–3 hàng/cột cùng lúc → nhân điểm ×2, ×3; xóa sạch bảng → ×5
- Game kết thúc khi không còn block nào trong tray đặt được
