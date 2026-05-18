# Game Launcher

Đây là launcher GUI để chạy BlockPuzzleGame.exe trên Windows.

## Tính năng

- 🎮 Giao diện GUI đơn giản
- ▶️ Nút "Play" để khởi động game
- 📊 Hiển thị trạng thái game
- ✅ Kiểm tra file game tồn tại
- ⚙️ Theo dõi tiến trình game

## Cách dùng

### Yêu cầu
- Visual Studio 2022 (hoặc CMake + compiler tương thích)
- SFML 2.5+ (phải cài đặt tại C:/SFML)

### Build Launcher

**Cách 1: Dùng script batch**
```bash
build_launcher.bat
```

**Cách 2: Build thủ công**
```bash
mkdir build_launcher
cd build_launcher
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Chạy Launcher

```bash
GameLauncher.exe
```

Hoặc chạy trực tiếp từ release folder:
```bash
build_launcher\Release\GameLauncher.exe
```

## Cấu trúc Launcher

```
launcher/
├── main.cpp          # Entry point, GUI rendering
├── GameLauncher.h    # Header file
└── GameLauncher.cpp  # Implementation - xử lý process Windows
```

## Cách hoạt động

1. **GameLauncher class**: Dùng Windows API (`CreateProcessA`) để launch BlockPuzzleGame.exe
2. **Theo dõi process**: Kiểm tra xem game còn chạy không bằng `GetExitCodeProcess`
3. **GUI**: SFML render button và status text
4. **Error handling**: Kiểm tra file tồn tại, hiển thị lỗi nếu cần

## Lưu ý

- Font: Launcher cần file `assets/arial.ttf` để hiển thị text đẹp
- Nếu không có font, text sẽ hiển thị bình thường nhưng không có font
- BlockPuzzleGame.exe phải cùng folder với GameLauncher.exe hoặc trong PATH
