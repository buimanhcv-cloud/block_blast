# Ghost Block Preview - Tối Ưu Hóa Trải Nghiệm Game

## 📋 Tính Năng Mới

Khi kéo một block trên board, các ô sẽ hiển thị **GHOST CELLS** (ô mờ) để:
- ✅ Hiển thị nơi block sẽ được đặt
- ✅ Ô hiển thị màu xanh lam nhạt + trong suốt (alpha = 80)
- ✅ Chỉ hiển thị khi vị trí hợp lệ
- ✅ Tự động ẩn nếu đặt block ở vị trí không hợp lệ

## 🔧 Các Thay Đổi Code

### 1. **Board.h** - Thêm method mới
```cpp
void renderGhostCells(sf::RenderWindow& window, const Block& block, sf::Vector2i position) const;
```

### 2. **Board.cpp** - Implement renderGhostCells
- Render ô mờ từng ô trong block shape
- Màu: `sf::Color(100, 150, 200, 80)` - xanh nhạt, 80/255 alpha (mờ)
- Outline: `sf::Color(150, 200, 255, 150)` - viền sáng hơn
- Bỏ qua ô nếu:
  - Nằm ngoài board (< 0 hoặc >= SIZE)
  - Ô đã có block (grid[x][y] == 1)

### 3. **GameManager.cpp** - Render ghost khi kéo
```cpp
if (draggingIndex >= 0 && trayBlocks[draggingIndex]) {
    const sf::Vector2f blockPos = trayBlocks[draggingIndex]->getPosition();
    const sf::Vector2i boardPos = board->getBoardPosition(blockPos);
    const bool canPlace = board->canPlaceBlock(...);
    
    if (canPlace) {
        board->renderGhostCells(window, *trayBlocks[draggingIndex], boardPos);
    }
}
```

## 🎮 Cách Dùng

1. **Build game:**
```bash
build_and_run.bat
```

2. **Chơi game:**
   - Kéo block từ tray xuống board
   - Xem ô mờ (ghost cells) hiển thị vị trí đặt
   - Thả block để hoàn tất

## 📊 Màu Sắc

| Loại | RGB | Alpha | Mục Đích |
|------|-----|-------|----------|
| Fill | (100, 150, 200) | 80 | Preview fill - mờ |
| Outline | (150, 200, 255) | 150 | Preview border - sáng hơn |

## ✅ Kiểm Tra

- ✅ Ghost cells chỉ hiển thị khi block ở vị trí hợp lệ
- ✅ Ghost cells biến mất nếu block ở vị trí không hợp lệ
- ✅ Ghost cells không đặt lên block đã tồn tại
- ✅ Ghost cells không hiển thị ngoài board
- ✅ Không ảnh hưởng đến game logic, chỉ là UI preview

## 📝 Lưu Ý

- Không thay đổi game logic, chỉ tối ưu UI
- Ghost cells render trước block drag, nên block luôn hiển thị trên top
- Performance tốt vì chỉ render khi dragging
