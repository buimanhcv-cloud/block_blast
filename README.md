# Block Puzzle Game

Game xếp hình block viết bằng C++ và SFML 2.5.

## Tính năng
- 19 loại hình block (single, domino, triomino, tetromino, pentomino)
- Tray 3 block cùng lúc — đặt hết 3 mới nhận block mới
- Tính điểm combo khi xóa nhiều hàng liên tiếp
- High score lưu trong phiên chơi
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
- Xóa nhiều hàng liên tiếp → combo x2, x3...
- Game kết thúc khi không còn block nào trong tray đặt được
