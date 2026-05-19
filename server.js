const net = require('net');
const { EventEmitter } = require('events');

const MESSAGE_TYPES = {
    HANDSHAKE: 0x01,
    JOIN_GAME: 0x02,
    MOVE: 0x03,
    SCORE_UPDATE: 0x04,
    GAME_STATE: 0x05,
    TIME_UPDATE: 0x06,
    GAME_OVER: 0x07,
    OPPONENT_DISCONNECT: 0x08,
    PING: 0x09,
    PONG: 0x0A,
    ERROR_MSG: 0xFF
};

class GameSession {
    constructor(sessionId, player1) {
        this.sessionId = sessionId;
        this.player1 = player1;
        this.player2 = null;
        this.gameMode = null; // 'realtime' or 'turn_based'
        this.timeMode = null; // 60, 180, 300 (seconds)
        this.player1Score = 0;
        this.player2Score = 0;
        this.player1Board = new Array(64).fill(0);
        this.player2Board = new Array(64).fill(0);
        this.player1Time = 0;
        this.player2Time = 0;
        this.gameStartTime = Date.now();
        this.gameActive = true;
    }

    isFull() {
        return this.player1 && this.player2;
    }

    addPlayer(player) {
        if (!this.player2) {
            this.player2 = player;
            this.gameStartTime = Date.now();
            return true;
        }
        return false;
    }

    getOpponent(player) {
        if (player === this.player1) return this.player2;
        if (player === this.player2) return this.player1;
        return null;
    }

    removePlayer(player) {
        if (player === this.player1) this.player1 = null;
        if (player === this.player2) this.player2 = null;
        this.gameActive = this.player1 || this.player2; // Keep game if any player remains
    }

    isGameOver() {
        return !this.gameActive || (!this.player1 && !this.player2);
    }
}

class BlockPuzzleServer extends EventEmitter {
    constructor(port = 9000) {
        super();
        this.port = port;
        this.server = null;
        this.sessions = new Map();
        this.players = new Map();
        this.waitingPlayers = [];
        this.sessionCounter = 0;
    }

    start() {
        this.server = net.createServer((socket) => {
            console.log(`Client connected: ${socket.remoteAddress}:${socket.remotePort}`);
            
            let player = {
                socket: socket,
                playerId: `player_${Date.now()}_${Math.random()}`,
                sessionId: null,
                name: 'Unknown',
                connected: true
            };

            this.players.set(player.playerId, player);

            socket.on('data', (data) => {
                this.handleClientMessage(player, data);
            });

            socket.on('end', () => {
                console.log(`Client disconnected: ${player.playerId}`);
                this.handlePlayerDisconnect(player);
            });

            socket.on('error', (err) => {
                console.error(`Socket error for ${player.playerId}:`, err);
                this.handlePlayerDisconnect(player);
            });
        });

        this.server.listen(this.port, () => {
            console.log(`Block Puzzle Game Server listening on port ${this.port}`);
        });
    }

    handleClientMessage(player, data) {
        try {
            if (data.length < 5) return; // Minimum message size

            const lengthBuf = data.slice(0, 4);
            const msgLength = (lengthBuf[0] << 24) | (lengthBuf[1] << 16) |
                              (lengthBuf[2] << 8) | lengthBuf[3];

            if (data.length < 4 + msgLength) return; // Incomplete message

            const msgData = data.slice(4, 4 + msgLength);
            const msgType = msgData[0];

            switch (msgType) {
                case MESSAGE_TYPES.HANDSHAKE:
                    this.handleHandshake(player, msgData);
                    break;
                case MESSAGE_TYPES.JOIN_GAME:
                    this.handleJoinGame(player, msgData);
                    break;
                case MESSAGE_TYPES.MOVE:
                    this.handleMove(player, msgData);
                    break;
                case MESSAGE_TYPES.SCORE_UPDATE:
                    this.handleScoreUpdate(player, msgData);
                    break;
                case MESSAGE_TYPES.TIME_UPDATE:
                    this.handleTimeUpdate(player, msgData);
                    break;
                default:
                    console.log(`Unknown message type: ${msgType}`);
            }
        } catch (err) {
            console.error('Error handling message:', err);
        }
    }

    handleHandshake(player, msgData) {
        const nameBytes = msgData.slice(1);
        player.name = nameBytes.toString('utf8');
        console.log(`Player ${player.playerId} identified as: ${player.name}`);

        // Try to match with waiting player
        if (this.waitingPlayers.length > 0) {
            const waitingPlayer = this.waitingPlayers.shift();
            this.createGameSession(waitingPlayer, player);
        } else {
            this.waitingPlayers.push(player);
            this.sendMessage(player, MESSAGE_TYPES.PING, Buffer.from([0x00]));
        }
    }

    createGameSession(player1, player2) {
        const sessionId = ++this.sessionCounter;
        const session = new GameSession(sessionId, player1);
        session.addPlayer(player2);
        session.gameMode = 'realtime'; // Default mode
        session.timeMode = 180; // Default 3 minutes

        this.sessions.set(sessionId, session);
        player1.sessionId = sessionId;
        player2.sessionId = sessionId;

        console.log(`Game session ${sessionId} created: ${player1.name} vs ${player2.name}`);

        // Notify both players that game started
        this.sendMessage(player1, MESSAGE_TYPES.JOIN_GAME, Buffer.from([0x01])); // 0x01 = you're player 1
        this.sendMessage(player2, MESSAGE_TYPES.JOIN_GAME, Buffer.from([0x02])); // 0x02 = you're player 2
    }

    handleJoinGame(player, msgData) {
        // Players can specify game mode/time mode here if needed
        console.log(`${player.name} joining game...`);
    }

    handleMove(player, msgData) {
        if (!player.sessionId || !this.sessions.has(player.sessionId)) return;

        const session = this.sessions.get(player.sessionId);
        const opponent = session.getOpponent(player);
        if (!opponent) return;

        // Forward move to opponent
        this.sendMessage(opponent, MESSAGE_TYPES.MOVE, msgData.slice(1));
    }

    handleScoreUpdate(player, msgData) {
        if (!player.sessionId || !this.sessions.has(player.sessionId)) return;

        const session = this.sessions.get(player.sessionId);
        if (player === session.player1) {
            session.player1Score = (msgData[1] << 24) | (msgData[2] << 16) |
                                   (msgData[3] << 8) | msgData[4];
        } else {
            session.player2Score = (msgData[1] << 24) | (msgData[2] << 16) |
                                   (msgData[3] << 8) | msgData[4];
        }

        const opponent = session.getOpponent(player);
        if (opponent) {
            this.sendMessage(opponent, MESSAGE_TYPES.SCORE_UPDATE, msgData.slice(1));
        }
    }

    handleTimeUpdate(player, msgData) {
        if (!player.sessionId || !this.sessions.has(player.sessionId)) return;

        const session = this.sessions.get(player.sessionId);
        const opponent = session.getOpponent(player);
        if (!opponent) return;

        // Forward time update to opponent
        this.sendMessage(opponent, MESSAGE_TYPES.TIME_UPDATE, msgData.slice(1));
    }

    handlePlayerDisconnect(player) {
        player.connected = false;

        if (player.sessionId && this.sessions.has(player.sessionId)) {
            const session = this.sessions.get(player.sessionId);
            const opponent = session.getOpponent(player);

            if (opponent && opponent.connected) {
                // Notify opponent of disconnect
                this.sendMessage(opponent, MESSAGE_TYPES.OPPONENT_DISCONNECT,
                    Buffer.from([0x01])); // 0x01 = opponent left
            }

            session.removePlayer(player);
            if (session.isGameOver()) {
                this.sessions.delete(player.sessionId);
            }
        }

        // Remove from waiting list
        const idx = this.waitingPlayers.indexOf(player);
        if (idx > -1) {
            this.waitingPlayers.splice(idx, 1);
        }

        this.players.delete(player.playerId);
    }

    sendMessage(player, messageType, payload = Buffer.alloc(0)) {
        if (!player.socket || !player.connected) return;

        try {
            const msgData = Buffer.concat([
                Buffer.from([messageType]),
                payload
            ]);

            const lengthBuf = Buffer.alloc(4);
            lengthBuf.writeUInt32BE(msgData.length, 0);

            player.socket.write(Buffer.concat([lengthBuf, msgData]));
        } catch (err) {
            console.error('Error sending message:', err);
        }
    }

    stop() {
        if (this.server) {
            this.server.close();
            console.log('Server stopped');
        }
    }

    getStatus() {
        return {
            activeSessions: this.sessions.size,
            connectedPlayers: this.players.size,
            waitingPlayers: this.waitingPlayers.length
        };
    }
}

// Start server
const server = new BlockPuzzleServer(9000);
server.start();

// Status updates
setInterval(() => {
    console.log('Server status:', server.getStatus());
}, 30000);

// Graceful shutdown
process.on('SIGINT', () => {
    console.log('Shutting down server...');
    server.stop();
    process.exit(0);
});
