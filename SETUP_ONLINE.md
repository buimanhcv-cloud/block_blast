# Setup Guide: Online Multiplayer Mode

## ⚙️ Prerequisites

### Windows
- **SFML 2.5+**: Download from https://www.sfml-dev.org/download.php
  - Extract to `C:\SFML`
- **GCC Compiler**: TDM-GCC-64 or MinGW-w64
  - Usually included with Dev-C++
- **Node.js 14+**: Download from https://nodejs.org
  - Install globally
- **Game build**: Run `build_windows.bat`

### Linux / Mac
- Install SFML dev libraries: `apt-get install libsfml-dev` or `brew install sfml`
- Node.js: `apt-get install nodejs npm` or `brew install node`
- Modify CMakeLists.txt and build_windows.bat for your platform

## 🚀 Quick Start

### Step 1: Build Game
```bash
cd C:\Users\buima\Documents\app
build_windows.bat
```

Expected output:
```
Build SUCCESS: BlockPuzzleGame.exe
```

### Step 2: Start Server
```bash
cd C:\Users\buima\Documents\app
node server.js
```

Expected output:
```
Block Puzzle Game Server listening on port 9000
```

⚠️ **Keep this terminal open!**

### Step 3: Launch Game Instances

**Terminal 3 (Player 1):**
```bash
BlockPuzzleGame.exe
```

**Terminal 4 (Player 2):**
```bash
BlockPuzzleGame.exe
```

Or use helper script:
```bash
test_online.bat
```

## 🎮 Game Flow

1. **Menu**: Click "Online Đối chiến" (red/pink button)

2. **Mode Selection**: 
   - Choose **Realtime** (simultaneous play) or **Turn-based** (chess clock)
   - Click or arrow keys to select

3. **Time Mode** (if turn-based):
   - Choose: Fast (1m), Normal (3m), or Long (5m)

4. **Server IP**:
   - Local play: `127.0.0.1`
   - LAN: `192.168.x.x` (your local IP)
   - Internet: Need port forwarding or VPN

5. **Waiting Screen**:
   - "Waiting for opponent..."
   - When 2 players connected → Game starts!

## 🌐 Network Configuration

### Local Testing (Same Machine)
```
Server: 127.0.0.1:9000
Client 1: 127.0.0.1
Client 2: 127.0.0.1
```
Works out of the box! ✅

### LAN Testing (Multiple Machines)
```
1. Find server machine's IP: ipconfig (Windows)
   Look for "IPv4 Address" (e.g., 192.168.1.100)

2. Start server on machine A:
   node server.js

3. On machine B & C, connect to: 192.168.1.100
```

### Internet (Remote Players)
```
⚠️ Requires port forwarding or cloud hosting

Server Setup:
1. Deploy server.js to cloud (AWS, Heroku, etc.)
2. Use cloud service IP/domain as game connect address

Router Setup (Alternative):
1. Port forward 9000 to server machine
2. Get your public IP: https://whatsmyip.com
3. Clients connect to: YOUR_PUBLIC_IP
```

## 🔧 Troubleshooting

### "Connection failed" Error
```
Checklist:
✓ server.js running? (check terminal)
✓ IP correct? (use 127.0.0.1 for local)
✓ Port 9000 open? (firewall)
✓ Both players connecting to SAME IP?
```

### Game Freezes After Connecting
```
Solution:
- Make sure online.cpp/OnlineGameMode is properly initialized
- Check console for error messages
- Restart both game instances
```

### Desynchronization (Moves not synced)
```
Diagnosis:
- Latency too high (> 200ms)
- Network packet loss
- Server crash (restart server)

Solution:
- Use LAN (ping < 50ms ideal)
- Restart game & server
- Check network stability: ping 127.0.0.1
```

### Server Crashes
```
Reason: Usually JavaScript error
Fix: Check terminal output, look for stack trace
Contact: Check console for "Error handling message:"
```

## 📊 Server Monitoring

While server is running, every 30 seconds you'll see:
```
Server status: {
  activeSessions: 5,
  connectedPlayers: 10,
  waitingPlayers: 2
}
```

This shows:
- `activeSessions`: Current games in progress
- `connectedPlayers`: Total connected players
- `waitingPlayers`: Players waiting for opponent

## 🎯 Testing Scenarios

### Scenario 1: Local Realtime PvP
```
1. Terminal 1: node server.js
2. Terminal 2: BlockPuzzleGame.exe → Online → Realtime
3. Terminal 3: BlockPuzzleGame.exe → Online → Realtime
4. Both connect to 127.0.0.1
5. Both boards visible side-by-side
6. Scores update in real-time
```

### Scenario 2: LAN Turn-based (Chess Clock)
```
1. Machine A (192.168.1.100): node server.js
2. Machine B: Game → Online → Turn-based → Normal (3m)
3. Machine C: Game → Online → Turn-based → Normal (3m)
4. Connect to: 192.168.1.100
5. Timer shown for active player
6. Every time limit expires → switch turn
```

### Scenario 3: Test Disconnection
```
1. Start online game normally
2. Close one game window mid-match
3. Other player sees: "Opponent Disconnected"
4. Can return to menu
```

## 📝 File Locations

| File | What |
|------|------|
| `BlockPuzzleGame.exe` | Game executable |
| `server.js` | Server code |
| `ONLINE_PVP_IMPLEMENTATION.md` | Technical docs |
| `ONLINE_MULTIPLAYER.md` | Protocol specs |

## ❌ Common Mistakes

❌ **Mistake**: Starting game before server
```
✅ Fix: Always start server first (node server.js)
```

❌ **Mistake**: Using wrong IP
```
✅ Fix: Use 127.0.0.1 for local, not "localhost" or "127.0.0.1:9000"
```

❌ **Mistake**: Not building game after code changes
```
✅ Fix: Always run build_windows.bat after modifying code
```

❌ **Mistake**: Multiple servers on same port
```
✅ Fix: Kill previous node.js: taskkill /F /IM node.exe
```

## ✅ Verification Checklist

After everything set up:

- [ ] SFML installed in C:\SFML
- [ ] Node.js installed (test: `node --version`)
- [ ] Game built successfully (`BlockPuzzleGame.exe` exists)
- [ ] Server starts without errors (`node server.js`)
- [ ] Can launch 2 game instances
- [ ] Both see "Online Đối chiến" menu
- [ ] Both can input server IP
- [ ] Connection succeeds (no timeout)
- [ ] Game starts automatically when 2 connected

## 🎓 Learning Resources

- **TCP Networking**: https://en.wikipedia.org/wiki/Transmission_Control_Protocol
- **SFML Documentation**: https://www.sfml-dev.org/documentation/
- **Node.js Guide**: https://nodejs.org/en/docs/guides/
- **Game Networking**: https://gafferongames.com/

## 📞 Support

If stuck:
1. Check **Troubleshooting** section above
2. Read `ONLINE_PVP_IMPLEMENTATION.md` for technical details
3. Check server terminal for error messages
4. Verify network connectivity: `ping 127.0.0.1`
5. Ensure ports not blocked: `netstat -an | find "9000"`

---

**Ready to play?** 🎮 Good luck!
