/**
 * Block Puzzle Game – Online Multiplayer Server
 * WebSocket binary protocol (ws library)
 * Compatible with: Koyeb, Render, Railway, Glitch, Fly.io (any HTTP host)
 *
 * Usage:  node server.js [port]   (default: 5000)
 * Deploy: set PORT env variable on hosting platform
 *
 * Wire format per message: [1 byte type][2 bytes payload length LE][N bytes payload]
 */

'use strict';

const http = require('http');
const { WebSocketServer, OPEN } = require('ws');

const PORT = parseInt(process.env.PORT || process.argv[2] || '5000', 10);

// ── Message type constants (must match C++ MsgType enum) ─────────────────────
const MSG = {
    JOIN:         0x01,
    MOVE:         0x02,
    READY:        0x03,
    PING:         0x04,
    ROOM_JOINED:  0x10,
    GAME_START:   0x11,
    GAME_STATE:   0x12,
    SCORE_UPDATE: 0x13,
    TIME_UPDATE:  0x14,
    YOUR_TURN:    0x15,
    GAME_OVER:    0x16,
    PONG:         0x17,
    ERROR_MSG:    0x18,
};

const BOARD_SIZE = 8;
const rooms = new Map();

// ── Packet builder ────────────────────────────────────────────────────────────
function buildPacket(type, payload) {
    const buf = Buffer.alloc(3 + payload.length);
    buf[0] = type;
    buf.writeUInt16LE(payload.length, 1);
    payload.copy(buf, 3);
    return buf;
}

// ── Room ──────────────────────────────────────────────────────────────────────
class Room {
    constructor(id, mode, timeLimitSec) {
        this.id           = id;
        this.mode         = mode;
        this.timeLimitSec = timeLimitSec;
        this.players      = [];
        this.started      = false;
        this.currentTurn  = 0;
        this.timers       = [timeLimitSec * 1000, timeLimitSec * 1000];
        this.scores       = [0, 0];
        this.combos       = [0, 0];
        this.boards       = [
            Array.from({length: BOARD_SIZE}, () => new Array(BOARD_SIZE).fill(0)),
            Array.from({length: BOARD_SIZE}, () => new Array(BOARD_SIZE).fill(0)),
        ];
        this.tickInterval = null;
    }

    isFull()  { return this.players.length >= 2; }
    isEmpty() { return this.players.length === 0; }

    addPlayer(player) {
        const pid = this.players.length;
        player.playerId = pid;
        player.room = this;
        this.players.push(player);
        return pid;
    }

    removePlayer(player) {
        this.players = this.players.filter(p => p !== player);
        if (this.tickInterval) {
            clearInterval(this.tickInterval);
            this.tickInterval = null;
        }
    }

    broadcast(buf, excludeId = -1) {
        for (const p of this.players) {
            if (p.playerId !== excludeId && p.ws.readyState === OPEN)
                p.ws.send(buf);
        }
    }

    startGame() {
        this.started = true;
        const payload = Buffer.alloc(3);
        payload[0] = this.mode;
        // timeLimitSec as 2 bytes LE (matches updated C++ sendJoin)
        payload.writeUInt16LE(this.timeLimitSec, 1);
        this.broadcast(buildPacket(MSG.GAME_START, payload));

        if (this.mode === 1)
            this.players[0].ws.send(buildPacket(MSG.YOUR_TURN, Buffer.alloc(0)));

        this.tickInterval = setInterval(() => this.tick(), 500);
    }

    tick() {
        if (!this.started) return;
        const TICK = 500;
        if (this.mode === 0) {
            this.timers[0] = Math.max(0, this.timers[0] - TICK);
            this.timers[1] = Math.max(0, this.timers[1] - TICK);
        } else {
            this.timers[this.currentTurn] = Math.max(0, this.timers[this.currentTurn] - TICK);
        }
        this.broadcastTimeUpdate();
        if (this.timers[0] <= 0 || this.timers[1] <= 0) {
            this.endGame(this.timers[0] <= 0 ? 1 : 0);
        }
    }

    broadcastTimeUpdate() {
        const buf = Buffer.alloc(8);
        buf.writeUInt32LE(this.timers[0], 0);
        buf.writeUInt32LE(this.timers[1], 4);
        this.broadcast(buildPacket(MSG.TIME_UPDATE, buf));
    }

    applyMove(playerId, boardX, boardY, shape) {
        const board = this.boards[playerId];
        for (let r = 0; r < shape.length; r++)
            for (let c = 0; c < shape[r].length; c++)
                if (shape[r][c]) {
                    const x = boardX + c, y = boardY + r;
                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
                        board[x][y] = 1;
                }

        let linesCleared = 0;
        for (let y = 0; y < BOARD_SIZE; y++) {
            if (board.every(col => col[y])) {
                board.forEach(col => col[y] = 0);
                linesCleared++;
            }
        }
        for (let x = 0; x < BOARD_SIZE; x++) {
            if (board[x].every(v => v)) {
                board[x].fill(0);
                linesCleared++;
            }
        }

        if (linesCleared > 0) {
            this.combos[playerId]++;
            this.scores[playerId] += linesCleared * 10 * this.combos[playerId];
        } else {
            this.combos[playerId] = 0;
        }

        // Send board snapshot to opponent
        const boardBuf = Buffer.alloc(BOARD_SIZE * BOARD_SIZE);
        for (let y = 0; y < BOARD_SIZE; y++)
            for (let x = 0; x < BOARD_SIZE; x++)
                boardBuf[y * BOARD_SIZE + x] = board[x][y];
        this.broadcast(buildPacket(MSG.GAME_STATE, boardBuf), playerId);

        // Score update
        const scoreBuf = Buffer.alloc(6);
        scoreBuf[0] = playerId;
        scoreBuf.writeUInt32LE(this.scores[playerId], 1);
        scoreBuf[5] = Math.min(this.combos[playerId], 255);
        this.broadcast(buildPacket(MSG.SCORE_UPDATE, scoreBuf));

        // Turn switch
        if (this.mode === 1) {
            this.currentTurn = 1 - this.currentTurn;
            const next = this.players[this.currentTurn];
            if (next && next.ws.readyState === OPEN)
                next.ws.send(buildPacket(MSG.YOUR_TURN, Buffer.alloc(0)));
        }
    }

    endGame(winnerId) {
        if (this.tickInterval) { clearInterval(this.tickInterval); this.tickInterval = null; }
        this.started = false;
        const buf = Buffer.alloc(1);
        buf[0] = winnerId;
        this.broadcast(buildPacket(MSG.GAME_OVER, buf));
        console.log(`[Room ${this.id}] Game over. Winner: player ${winnerId}`);
        rooms.delete(this.id);
    }
}

// ── Player ────────────────────────────────────────────────────────────────────
class Player {
    constructor(ws) {
        this.ws       = ws;
        this.playerId = -1;
        this.room     = null;
        this.recvBuf  = Buffer.alloc(0);
    }

    // WebSocket delivers full binary frames — but we still frame-parse
    // in case multiple game packets arrive in one WS message
    onMessage(data) {
        this.recvBuf = Buffer.concat([this.recvBuf, data]);
        while (this.recvBuf.length >= 3) {
            const payloadLen = this.recvBuf.readUInt16LE(1);
            const totalLen   = 3 + payloadLen;
            if (this.recvBuf.length < totalLen) break;
            const type    = this.recvBuf[0];
            const payload = this.recvBuf.slice(3, totalLen);
            this.recvBuf  = this.recvBuf.slice(totalLen);
            handleMessage(this, type, payload);
        }
    }
}

// ── Message handler ───────────────────────────────────────────────────────────
function handleMessage(player, type, payload) {
    switch (type) {
    case MSG.JOIN: {
        if (payload.length < 4) break;
        const mode         = payload[0];
        // timeLimitSec now 2 bytes LE
        const timeLimitSec = payload.readUInt16LE(1);
        const roomIdLen    = payload[3];
        const roomId       = payload.slice(4, 4 + roomIdLen).toString('ascii');

        let room = rooms.get(roomId);
        if (!room) {
            room = new Room(roomId, mode, timeLimitSec);
            rooms.set(roomId, room);
            console.log(`[Server] Created room "${roomId}" mode=${mode} time=${timeLimitSec}s`);
        }
        if (room.isFull()) {
            player.ws.send(buildPacket(MSG.ERROR_MSG, Buffer.from('Room full')));
            break;
        }
        const pid = room.addPlayer(player);
        console.log(`[Room ${roomId}] Player ${pid} joined`);

        const joinBuf = Buffer.alloc(1);
        joinBuf[0] = pid;
        player.ws.send(buildPacket(MSG.ROOM_JOINED, joinBuf));

        if (room.isFull()) {
            console.log(`[Room ${roomId}] Starting game`);
            room.startGame();
        }
        break;
    }
    case MSG.MOVE: {
        if (!player.room || !player.room.started) break;
        if (payload.length < 4) break;
        const bx = payload[0], by = payload[1];
        const rows = payload[2], cols = payload[3];
        const shape = [];
        let idx = 4;
        for (let r = 0; r < rows; r++) {
            const row = [];
            for (let c = 0; c < cols; c++)
                row.push(idx < payload.length ? payload[idx++] : 0);
            shape.push(row);
        }
        player.room.applyMove(player.playerId, bx, by, shape);
        break;
    }
    case MSG.PING:
        player.ws.send(buildPacket(MSG.PONG, Buffer.alloc(0)));
        break;
    case MSG.READY:
        break;
    default:
        console.warn(`[Server] Unknown msg type: 0x${type.toString(16)}`);
    }
}

// ── HTTP + WebSocket server ───────────────────────────────────────────────────
const httpServer = http.createServer((req, res) => {
    // Health check endpoint (Koyeb/Render ping this)
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('Block Puzzle Game Server OK\n');
});

const wss = new WebSocketServer({ server: httpServer });

wss.on('connection', (ws, req) => {
    const player = new Player(ws);
    console.log(`[Server] Client connected: ${req.socket.remoteAddress}`);

    ws.on('message', data => player.onMessage(Buffer.from(data)));
    ws.on('error',   err  => console.error('[WS] Error:', err.message));
    ws.on('close',   ()   => {
        console.log(`[Server] Client disconnected: player ${player.playerId}`);
        if (player.room) {
            const room = player.room;
            room.removePlayer(player);
            if (room.started && room.players.length > 0) {
                room.endGame(room.players[0].playerId);
            } else if (room.isEmpty()) {
                rooms.delete(room.id);
            }
        }
    });
});

httpServer.listen(PORT, '0.0.0.0', () => {
    console.log(`[Server] Listening on port ${PORT} (WebSocket + HTTP)`);
    console.log(`[Server] Waiting for players...`);
});
