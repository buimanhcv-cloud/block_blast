# Block Puzzle Game - Online Multiplayer Mode

## Overview

Chế độ **Online Multiplayer** cho phép 2 người chơi thi đấu online qua mạng TCP.

### Tính năng

- **Hybrid Mode**: Hỗ trợ cả **Realtime** (cùng lúc) và **Turn-based** (lượt lượt với chess clock)
- **Time Modes**: Fast (1 phút), Normal (3 phút), Long (5 phút)
- **Real-time Synchronization**: Điểm, bảng, và thời gian được đồng bộ hóa
- **Server-based Matchmaking**: Kết nối 2 người chơi tự động từ server

## Server Setup

### Requirements
- Node.js 14+ ([https://nodejs.org](https://nodejs.org))

### Installation & Running

```bash
# Navigate to project root
cd /path/to/app

# Start server
node server.js
```

Server sẽ lắng nghe trên port **9000** (có thể thay đổi trong `server.js` dòng cuối cùng).

Output:
```
Block Puzzle Game Server listening on port 9000
```

## Client (Game) Integration

### Starting Online Game

1. **Launch Game**: Run `BlockPuzzleGame.exe`
2. **Select Mode**: Menu → Online Mode
3. **Choose Type**:
   - Realtime (cả hai chơi đồng thời)
   - Turn-based (lượt lượt với timer)
4. **Set Time** (if turn-based):
   - Fast: 1 phút/người
   - Normal: 3 phút/người
   - Long: 5 phút/người
5. **Enter Server IP**: 
   - Local: `127.0.0.1` (cùng máy)
   - Network: `192.168.x.x` (LAN)
   - Remote: Cần port forwarding hoặc domain
6. **Wait for Opponent**: Server tìm kiếm đối thủ, khi đủ 2 người game bắt đầu

## Network Protocol

### Message Types

```
0x01 HANDSHAKE  - Gửi tên người chơi
0x02 JOIN_GAME  - Xác nhận tham gia game
0x03 MOVE       - Gửi move: x, y, block_type
0x04 SCORE      - Cập nhật điểm
0x05 GAME_STATE - Đồng bộ toàn bộ trạng thái
0x06 TIME_UPDATE- Cập nhật thời gian (turn-based)
0x07 GAME_OVER  - Kết thúc game (winner, reason)
0x08 OPPONENT_DISCONNECT - Đối thủ mất kết nối
0x09 PING/0x0A PONG - Keep-alive
```

### Message Format

```
[4 bytes: message length][1 byte: type][payload...]
```

Length: Big-endian uint32 (chiều dài payload + type byte)

## Architecture

### Client Components

- **NetworkClient** (`NetworkClient.h/cpp`)
  - TCP socket wrapper
  - Async send/receive threading
  - Message serialization

- **GameNetworkManager** (`GameNetworkManager.h/cpp`)
  - Abstracts network layer từ game logic
  - Manages callbacks cho move, score, time updates
  
- **OnlineGameMode** (`OnlineGameMode.h/cpp`)
  - Game logic cho online mode
  - Handles timer management (realtime vs turn-based)
  - Tracks opponent state

- **OnlineGameSelectScreen** (`OnlineGameSelectScreen.h/cpp`)
  - Menu để chọn game type, time mode, server IP
  - UI loop trước khi game bắt đầu

### Server Components

- **BlockPuzzleServer** (`server.js`)
  - TCP server nhận kết nối client
  - Quản lý game sessions
  - Relay messages giữa 2 player
  - Timeout/disconnect handling

## Gameplay Rules

### Realtime Mode
- Cả 2 người chơi cùng lúc trên bảng riêng
- Không giới hạn thời gian
- Ai có điểm cao hơn trong thời gian quy định thì thắng
- Hoặc ai khiến đối thủ không đặt được nữa thì thắng

### Turn-based Mode (Chess Clock)
- Player 1 chơi trong X phút
- Khi hết thời gian, chuyển sang Player 2
- Lần lượt cho đến khi ai hết thời gian hoặc không đặt được
- Winner: Người chơi lâu hơn HOẶC có điểm cao hơn

## Ví dụ Testing (Local Network)

### Setup 2 Clients on Same Machine

**Terminal 1** (Server):
```bash
node server.js
# Listening on port 9000
```

**Game 1** (Player 1):
1. Run game
2. Select Online → Realtime → Server IP: 127.0.0.1
3. Waiting...

**Game 2** (Player 2):
1. Run game second instance
2. Select Online → Realtime → Server IP: 127.0.0.1
3. Waiting...

Khi có 2 client, server tạo game session → Game bắt đầu

## Troubleshooting

### Connection Failed
- ✅ Server đang chạy? `node server.js`
- ✅ IP đúng? Localhost = `127.0.0.1`, LAN = `192.168.x.x`
- ✅ Port forwarding nếu chơi online: Port 9000 phải mở

### Latency Issues
- Realtime mode: Được thiết kế cho LAN/low-latency (< 100ms)
- Turn-based: OK cho bất kỳ latency nào

### Opponent Disconnected
- Game kết thúc, hiện thông báo
- Return to menu

## Future Improvements

- [ ] Lobby system (tạo/join game cụ thể)
- [ ] Ranked leaderboard
- [ ] Custom game settings
- [ ] Replay/spectator mode
- [ ] Mobile client
- [ ] Cross-platform support

## Development Notes

### Adding New Message Types

1. Thêm vào enum `MessageType` (NetworkClient.h)
2. Implement handler trong `GameNetworkManager::handleMessage()`
3. Add serialization logic nếu cần
4. Update server relay logic (server.js)

### Performance

- Messages: ~50 bytes mỗi cái (move)
- Update frequency: 60 FPS (client), 30 FPS (network sync)
- Bandwidth: < 50 KB/s per connection
