# Requirements Document

## Introduction

Tính năng Online Multiplayer Hybrid bổ sung chế độ chơi PvP (Player vs Player) trực tuyến cho game Block Puzzle 8×8 hiện có. Hệ thống hỗ trợ hai chế độ thời gian: **Realtime** (cả hai người chơi đặt block đồng thời) và **Turn-based** (lần lượt theo lượt với giới hạn thời gian kiểu cờ vua). Giao tiếp giữa client C++ (SFML) và server Node.js sử dụng giao thức Binary TCP với các message type định nghĩa sẵn (MOVE, SCORE, TIME_UPDATE, GAME_STATE). Giao diện hiển thị split-screen: bảng của người chơi bên trái, bảng đối thủ (thu nhỏ, read-only) bên phải, kèm đồng hồ kiểu cờ vua.

---

## Glossary

- **TCP_Client**: Thành phần C++ quản lý kết nối TCP tới server, gửi/nhận binary message bất đồng bộ.
- **Network_Manager**: Thành phần C++ điều phối hàng đợi message đến/đi, parse binary protocol, và phân phối sự kiện tới game logic.
- **Online_Game_Mode**: Thành phần C++ điều khiển luồng game online (Realtime hoặc Turn-based), xử lý trạng thái lượt, đồng hồ, và kết quả.
- **Game_Server**: Server Node.js quản lý session, đồng bộ trạng thái game giữa hai client, và phân xử kết quả.
- **Session**: Một phiên chơi PvP gồm đúng hai người chơi, một chế độ thời gian, và một trạng thái game dùng chung.
- **Lobby_Screen**: Màn hình C++/SFML cho phép người chơi chọn chế độ (Realtime/Turn-based), chọn thời gian (1/3/5 phút), và chờ đối thủ.
- **Split_Screen_HUD**: Giao diện SFML hiển thị bảng của người chơi (trái, đầy đủ tương tác) và bảng đối thủ (phải, thu nhỏ, read-only) cùng đồng hồ.
- **Chess_Clock**: Đồng hồ đếm ngược kiểu cờ vua hiển thị thời gian còn lại của mỗi người chơi.
- **Binary_Protocol**: Định dạng message nhị phân gồm: 1 byte message type + 4 byte payload length + N byte payload (JSON hoặc raw bytes).
- **MOVE_Message**: Binary message gửi khi người chơi đặt một block lên bảng, chứa block ID, tọa độ (x, y), và timestamp.
- **SCORE_Message**: Binary message phát sóng điểm số cập nhật của một người chơi sau mỗi lần đặt block hoặc xóa dòng.
- **TIME_UPDATE_Message**: Binary message server gửi định kỳ để đồng bộ thời gian còn lại của mỗi người chơi.
- **GAME_STATE_Message**: Binary message chứa snapshot đầy đủ trạng thái game (grid, điểm, lượt, thời gian) dùng để đồng bộ lại sau mất kết nối.
- **Realtime_Mode**: Chế độ cả hai người chơi đặt block đồng thời, không có khái niệm lượt.
- **Turn_Based_Mode**: Chế độ người chơi thay nhau đặt block; mỗi lượt có giới hạn thời gian riêng; hết giờ tự động chuyển lượt.
- **Time_Control**: Tổng thời gian mỗi người chơi được phép sử dụng trong một ván: 1 phút (nhanh), 3 phút (bình thường), 5 phút (dài).
- **Opponent_Board**: Bản sao read-only của bảng đối thủ, hiển thị ở phía phải màn hình với tỉ lệ thu nhỏ.

---

## Requirements

### Requirement 1: Kết nối TCP tới Server

**User Story:** As a người chơi, I want kết nối tới server game qua TCP, so that tôi có thể tham gia phiên chơi PvP trực tuyến.

#### Acceptance Criteria

1. WHEN người chơi chọn chế độ Online từ menu chính, THE Lobby_Screen SHALL hiển thị ô nhập địa chỉ server (IP:port) với giá trị mặc định `127.0.0.1:3000`.
2. WHEN người chơi xác nhận kết nối, THE TCP_Client SHALL thiết lập kết nối TCP tới server trong vòng 5 giây.
3. IF kết nối TCP thất bại sau 5 giây, THEN THE TCP_Client SHALL thông báo lỗi kết nối và cho phép người chơi thử lại mà không cần khởi động lại ứng dụng.
4. WHEN kết nối TCP được thiết lập thành công, THE TCP_Client SHALL gửi handshake message chứa player ID và phiên bản protocol.
5. IF server từ chối handshake do phiên bản protocol không tương thích, THEN THE TCP_Client SHALL hiển thị thông báo lỗi phiên bản và đóng kết nối.
6. WHILE kết nối TCP đang hoạt động, THE TCP_Client SHALL gửi heartbeat message mỗi 10 giây để duy trì kết nối.
7. IF không nhận được phản hồi heartbeat trong 15 giây, THEN THE TCP_Client SHALL coi kết nối đã mất và chuyển sang trạng thái reconnect.

---

### Requirement 2: Binary Protocol — Định nghĩa và Serialization

**User Story:** As a developer, I want một giao thức binary rõ ràng, so that client C++ và server Node.js có thể giao tiếp chính xác và hiệu quả.

#### Acceptance Criteria

1. THE Binary_Protocol SHALL định nghĩa header gồm: 1 byte message type (uint8), 4 byte payload length (uint32 big-endian), theo sau là N byte payload.
2. THE Binary_Protocol SHALL hỗ trợ các message type: `HANDSHAKE` (0x01), `MOVE` (0x02), `SCORE` (0x03), `TIME_UPDATE` (0x04), `GAME_STATE` (0x05), `TURN_CHANGE` (0x06), `GAME_OVER` (0x07), `HEARTBEAT` (0x08), `ERROR` (0x09).
3. WHEN THE Network_Manager serializes một MOVE_Message, THE Network_Manager SHALL tạo payload chứa: player_id (string), block_id (uint8), position_x (int8), position_y (int8), timestamp (uint64 milliseconds).
4. WHEN THE Network_Manager receives một binary message, THE Network_Manager SHALL parse header trước, kiểm tra payload length, rồi mới đọc payload.
5. IF payload length trong header không khớp với số byte thực tế nhận được, THEN THE Network_Manager SHALL bỏ qua message đó và ghi log lỗi.
6. FOR ALL message types được định nghĩa, serializing rồi deserializing SHALL tạo ra object tương đương với object gốc (round-trip property).
7. THE Network_Manager SHALL xử lý tất cả binary message bất đồng bộ trên một thread riêng biệt, không block main game loop.

---

### Requirement 3: Lobby và Matchmaking

**User Story:** As a người chơi, I want chọn chế độ chơi và chờ đối thủ trong lobby, so that tôi có thể bắt đầu ván đấu với cấu hình mong muốn.

#### Acceptance Criteria

1. THE Lobby_Screen SHALL hiển thị lựa chọn chế độ: Realtime và Turn-based.
2. THE Lobby_Screen SHALL hiển thị lựa chọn Time_Control: 1 phút, 3 phút, 5 phút.
3. WHEN người chơi xác nhận cấu hình và nhấn "Tìm trận", THE Game_Server SHALL ghép cặp người chơi đầu tiên đang chờ với cùng chế độ và Time_Control.
4. WHILE chờ đối thủ, THE Lobby_Screen SHALL hiển thị trạng thái "Đang tìm đối thủ..." và thời gian đã chờ.
5. IF không tìm được đối thủ trong 60 giây, THEN THE Game_Server SHALL thông báo timeout và cho phép người chơi hủy hoặc tiếp tục chờ.
6. WHEN hai người chơi được ghép cặp, THE Game_Server SHALL tạo Session mới, gán player role (Player 1 / Player 2), và gửi GAME_STATE_Message khởi tạo tới cả hai client.
7. WHEN THE Lobby_Screen nhận được GAME_STATE_Message khởi tạo, THE Lobby_Screen SHALL chuyển sang màn hình game mà không cần thao tác thêm từ người chơi.

---

### Requirement 4: Chế độ Realtime

**User Story:** As a người chơi, I want chơi đồng thời với đối thủ trong chế độ Realtime, so that tôi có thể cạnh tranh tốc độ đặt block và ghi điểm.

#### Acceptance Criteria

1. WHILE Realtime_Mode đang chạy, THE Online_Game_Mode SHALL cho phép cả hai người chơi đặt block đồng thời mà không cần chờ lượt.
2. WHEN người chơi đặt một block hợp lệ, THE Online_Game_Mode SHALL gửi MOVE_Message tới server trong vòng 50ms kể từ khi thao tác hoàn thành.
3. WHEN THE Game_Server nhận MOVE_Message, THE Game_Server SHALL validate nước đi, cập nhật trạng thái server, và broadcast SCORE_Message tới cả hai client.
4. WHEN THE Online_Game_Mode nhận SCORE_Message của đối thủ, THE Online_Game_Mode SHALL cập nhật Opponent_Board và điểm đối thủ trên Split_Screen_HUD.
5. WHILE Realtime_Mode đang chạy, THE Chess_Clock SHALL đếm ngược đồng thời cho cả hai người chơi từ giá trị Time_Control đã chọn.
6. WHEN đồng hồ của một người chơi về 0, THE Game_Server SHALL kết thúc ván đấu và gửi GAME_OVER_Message với kết quả thắng/thua/hòa.
7. IF người chơi không thể đặt thêm block nào (game over logic), THEN THE Online_Game_Mode SHALL gửi thông báo game over tới server và chờ kết quả chính thức từ GAME_OVER_Message.

---

### Requirement 5: Chế độ Turn-based

**User Story:** As a người chơi, I want chơi theo lượt với giới hạn thời gian mỗi lượt, so that tôi có thể suy nghĩ chiến thuật trong thời gian cho phép.

#### Acceptance Criteria

1. WHEN Ván đấu Turn-based bắt đầu, THE Game_Server SHALL chỉ định Player 1 đi trước và gửi TURN_CHANGE_Message tới cả hai client.
2. WHILE là lượt của người chơi, THE Online_Game_Mode SHALL cho phép người chơi đặt đúng một block rồi tự động kết thúc lượt.
3. WHILE là lượt của đối thủ, THE Online_Game_Mode SHALL khóa input đặt block của người chơi và hiển thị trạng thái "Lượt đối thủ".
4. WHEN người chơi đặt block trong lượt của mình, THE Online_Game_Mode SHALL gửi MOVE_Message và THE Game_Server SHALL chuyển lượt sang đối thủ bằng TURN_CHANGE_Message.
5. WHILE là lượt của người chơi, THE Chess_Clock SHALL chỉ đếm ngược đồng hồ của người chơi đó; đồng hồ đối thủ dừng lại.
6. IF đồng hồ của người chơi về 0 trong lượt của mình, THEN THE Game_Server SHALL tự động chuyển lượt sang đối thủ và ghi nhận lượt bị bỏ qua.
7. IF một người chơi bỏ qua lượt 3 lần liên tiếp do hết giờ, THEN THE Game_Server SHALL kết thúc ván đấu và tuyên bố người chơi còn lại thắng.
8. WHEN THE Online_Game_Mode nhận TURN_CHANGE_Message, THE Online_Game_Mode SHALL cập nhật trạng thái lượt và Chess_Clock trong vòng 100ms.

---

### Requirement 6: Đồng bộ Trạng thái Game (State Sync)

**User Story:** As a người chơi, I want trạng thái game luôn nhất quán giữa hai client, so that kết quả ván đấu là công bằng và chính xác.

#### Acceptance Criteria

1. THE Game_Server SHALL là nguồn sự thật duy nhất (single source of truth) cho trạng thái game; client chỉ hiển thị trạng thái do server xác nhận.
2. WHEN THE Game_Server nhận MOVE_Message, THE Game_Server SHALL validate nước đi theo luật game (block hợp lệ, vị trí trống) trước khi broadcast.
3. IF THE Game_Server phát hiện nước đi không hợp lệ, THEN THE Game_Server SHALL gửi ERROR_Message tới client vi phạm và rollback trạng thái local của client đó.
4. THE Game_Server SHALL gửi TIME_UPDATE_Message tới cả hai client mỗi 1 giây để đồng bộ thời gian còn lại.
5. IF một client mất kết nối và kết nối lại trong vòng 30 giây, THEN THE Game_Server SHALL gửi GAME_STATE_Message đầy đủ để client đồng bộ lại trạng thái hiện tại.
6. IF một client mất kết nối quá 30 giây, THEN THE Game_Server SHALL kết thúc ván đấu và tuyên bố người chơi còn lại thắng.
7. WHEN THE Network_Manager nhận GAME_STATE_Message, THE Network_Manager SHALL cập nhật toàn bộ trạng thái game local (grid, điểm, lượt, thời gian) từ snapshot trong message.

---

### Requirement 7: Split-Screen HUD và Chess Clock

**User Story:** As a người chơi, I want giao diện split-screen rõ ràng với đồng hồ đếm ngược, so that tôi luôn biết trạng thái ván đấu và thời gian còn lại.

#### Acceptance Criteria

1. THE Split_Screen_HUD SHALL chia màn hình 540×960 thành: vùng bảng người chơi (trái, đầy đủ tương tác, tỉ lệ gốc) và vùng Opponent_Board (phải, thu nhỏ 50%, read-only).
2. THE Split_Screen_HUD SHALL hiển thị Chess_Clock của người chơi bên dưới bảng của người chơi và Chess_Clock của đối thủ bên dưới Opponent_Board.
3. WHILE Chess_Clock đang đếm ngược, THE Chess_Clock SHALL cập nhật hiển thị mỗi 100ms với định dạng `MM:SS`.
4. WHEN thời gian còn lại dưới 10 giây, THE Chess_Clock SHALL đổi màu hiển thị sang đỏ để cảnh báo người chơi.
5. THE Split_Screen_HUD SHALL hiển thị điểm số của cả hai người chơi ở vị trí cố định phía trên mỗi bảng.
6. THE Split_Screen_HUD SHALL hiển thị tên/ID của mỗi người chơi phía trên bảng tương ứng.
7. WHILE là lượt của đối thủ (Turn-based), THE Split_Screen_HUD SHALL hiển thị overlay "Lượt đối thủ" mờ trên bảng của người chơi để ngăn tương tác.
8. WHEN ván đấu kết thúc, THE Split_Screen_HUD SHALL hiển thị màn hình kết quả với thắng/thua/hòa, điểm số cuối, và nút "Chơi lại" / "Về menu".

---

### Requirement 8: Network Manager — Xử lý Bất đồng bộ

**User Story:** As a developer, I want Network Manager xử lý message bất đồng bộ, so that network I/O không làm gián đoạn game loop 60fps.

#### Acceptance Criteria

1. THE Network_Manager SHALL chạy vòng lặp nhận message trên một `std::thread` riêng biệt, tách biệt hoàn toàn khỏi main game thread.
2. THE Network_Manager SHALL sử dụng thread-safe queue (bảo vệ bằng `std::mutex`) để chuyển message từ network thread sang game thread.
3. WHEN game thread gọi `Network_Manager::pollMessages()`, THE Network_Manager SHALL trả về tất cả message đang chờ trong queue mà không block.
4. THE Network_Manager SHALL giới hạn kích thước queue tối đa 1000 message; IF queue đầy, THEN THE Network_Manager SHALL bỏ qua message mới nhất và ghi log cảnh báo.
5. WHEN THE Network_Manager cần gửi message, THE Network_Manager SHALL đưa message vào send queue và gửi trên network thread, không gửi trực tiếp từ game thread.
6. THE Network_Manager SHALL đo latency round-trip của HEARTBEAT message và cung cấp giá trị latency hiện tại cho Split_Screen_HUD hiển thị.

---

### Requirement 9: Server Node.js — Session Management

**User Story:** As a developer, I want server Node.js quản lý session ổn định, so that nhiều ván đấu có thể diễn ra đồng thời mà không ảnh hưởng lẫn nhau.

#### Acceptance Criteria

1. THE Game_Server SHALL hỗ trợ tối thiểu 10 Session đồng thời mà không làm giảm hiệu năng xử lý message dưới 100ms.
2. THE Game_Server SHALL lưu trữ trạng thái mỗi Session trong bộ nhớ (in-memory) bao gồm: grid của cả hai người chơi, điểm số, lượt hiện tại, thời gian còn lại.
3. WHEN một Session kết thúc (game over hoặc disconnect), THE Game_Server SHALL giải phóng tài nguyên Session trong vòng 5 giây.
4. THE Game_Server SHALL validate mọi MOVE_Message theo luật game trước khi cập nhật trạng thái Session.
5. THE Game_Server SHALL ghi log mọi sự kiện quan trọng (kết nối, ngắt kết nối, game over, lỗi) với timestamp để hỗ trợ debug.
6. WHEN THE Game_Server khởi động, THE Game_Server SHALL lắng nghe kết nối TCP trên cổng có thể cấu hình (mặc định 3000).
7. IF THE Game_Server nhận message với message type không xác định, THEN THE Game_Server SHALL gửi ERROR_Message và bỏ qua message đó mà không crash.

---

### Requirement 10: Tích hợp vào Game Hiện có

**User Story:** As a developer, I want tính năng online tích hợp mượt mà vào codebase hiện có, so that single-player mode không bị ảnh hưởng.

#### Acceptance Criteria

1. THE GameManager SHALL mở rộng enum `GameState` để bao gồm `ONLINE_LOBBY` và `ONLINE_PLAYING` mà không thay đổi luồng `MENU → PLAYING → GAME_OVER` hiện có.
2. WHEN người chơi chọn chế độ Online, THE Menu SHALL hiển thị nút "Online Multiplayer" bổ sung mà không thay đổi layout nút hiện có.
3. THE Online_Game_Mode SHALL tái sử dụng `Board` và `Block` hiện có cho logic đặt block và xóa dòng mà không sửa đổi các class đó.
4. THE Online_Game_Mode SHALL tái sử dụng hệ thống âm thanh hiện có (place, clear, combo sounds) cho các sự kiện tương ứng trong chế độ online.
5. WHEN người chơi thoát khỏi chế độ Online (về menu hoặc đóng cửa sổ), THE TCP_Client SHALL đóng kết nối TCP một cách graceful trước khi giải phóng tài nguyên.
6. THE CMakeLists.txt SHALL được cập nhật để bao gồm tất cả file nguồn mới của tính năng online mà không phá vỡ build hiện tại.
