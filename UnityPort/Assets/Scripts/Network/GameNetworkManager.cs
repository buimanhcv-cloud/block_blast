using System;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace BlockBlast.Network
{
    using BlockBlast.Core;

    public enum OnlineMode
    {
        REALTIME = 0,
        TURN_BASED = 1
    }

    public enum TimePreset
    {
        FAST = 60,
        NORMAL = 180,
        LONG = 300
    }

    public class OnlineGameState
    {
        public int myScore = 0;
        public int opponentScore = 0;
        public int myCombo = 0;
        public int opponentCombo = 0;
        public int myTimeMs = 0;
        public int opponentTimeMs = 0;
        public bool myTurn = false;
        public bool gameOver = false;
        public int winnerId = -1;
        public int myPlayerId = -1;

        public int[,] opponentGrid = new int[Board.SIZE, Board.SIZE];
    }

    public class GameNetworkManager : MonoBehaviour
    {
        private NetworkClient client = new NetworkClient();
        private OnlineGameState state = new OnlineGameState();
        private bool wasConnected = false;

        public OnlineGameState State => state;
        public bool IsConnected => client.IsConnected;

        // --- Event Delegates ---
        public event Action<int> OnRoomJoined;
        public event Action<OnlineMode, int> OnGameStart;
        public event Action<OnlineGameState> OnStateUpdate;
        public event Action<int> OnGameOver;
        public event Action OnYourTurn;
        public event Action OnDisconnected;

        private void Update()
        {
            PollEvents();
        }

        private void OnDestroy()
        {
            Disconnect();
        }

        public async Task<bool> ConnectAsync(string host, int port)
        {
            state = new OnlineGameState();
            wasConnected = false;
            return await client.ConnectAsync(host, port);
        }

        public void Disconnect()
        {
            if (client.IsConnected)
            {
                client.Disconnect();
            }
        }

        public void JoinRoom(string roomId, OnlineMode mode, TimePreset timePreset)
        {
            if (client.IsConnected)
            {
                client.SendJoin(roomId, (int)mode, (int)timePreset);
            }
        }

        public void SendReady()
        {
            if (client.IsConnected) client.SendReady();
        }

        public void SendMove(int boardX, int boardY, int[,] shape)
        {
            if (client.IsConnected)
            {
                client.SendMove(boardX, boardY, shape);
                if (state.myTurn)
                {
                    state.myTurn = false; // Optimistic turn clearing
                }
            }
        }

        public void PollEvents()
        {
            bool isConn = client.IsConnected;
            if (isConn != wasConnected)
            {
                wasConnected = isConn;
                if (!isConn)
                {
                    OnDisconnected?.Invoke();
                }
            }

            if (!isConn) return;

            while (client.PollPacket(out NetPacket pkt))
            {
                HandlePacket(pkt);
            }
        }

        private void HandlePacket(NetPacket pkt)
        {
            switch (pkt.type)
            {
                case MsgType.ROOM_JOINED:
                    if (pkt.payload.Length >= 1)
                    {
                        state.myPlayerId = pkt.payload[0];
                        OnRoomJoined?.Invoke(state.myPlayerId);
                    }
                    break;

                case MsgType.GAME_START:
                    if (pkt.payload.Length >= 2)
                    {
                        OnlineMode mode = (OnlineMode)pkt.payload[0];
                        int timeSec = pkt.payload[1];
                        state.gameOver = false;
                        state.winnerId = -1;
                        state.myScore = 0;
                        state.opponentScore = 0;
                        state.myCombo = 0;
                        state.opponentCombo = 0;
                        state.myTurn = (mode == OnlineMode.TURN_BASED && state.myPlayerId == 0);

                        OnGameStart?.Invoke(mode, timeSec);
                    }
                    break;

                case MsgType.GAME_STATE:
                    // Payload is 64 bytes row-major grid representation of opponent board
                    if (pkt.payload.Length >= Board.SIZE * Board.SIZE)
                    {
                        for (int y = 0; y < Board.SIZE; y++)
                        {
                            for (int x = 0; x < Board.SIZE; x++)
                            {
                                // Grid index col-major (matching Board.cs structure)
                                state.opponentGrid[x, y] = pkt.payload[y * Board.SIZE + x];
                            }
                        }
                        OnStateUpdate?.Invoke(state);
                    }
                    break;

                case MsgType.SCORE_UPDATE:
                    // payload: [1 playerId][4 bytes score][1 byte combo]
                    if (pkt.payload.Length >= 6)
                    {
                        int pid = pkt.payload[0];
                        int score = BitConverter.ToInt32(pkt.payload, 1);
                        int combo = pkt.payload[5];

                        if (pid == state.myPlayerId)
                        {
                            state.myScore = score;
                            state.myCombo = combo;
                        }
                        else
                        {
                            state.opponentScore = score;
                            state.opponentCombo = combo;
                        }
                        OnStateUpdate?.Invoke(state);
                    }
                    break;

                case MsgType.TIME_UPDATE:
                    // payload: [4 bytes myTime ms][4 bytes oppTime ms]
                    if (pkt.payload.Length >= 8)
                    {
                        int t1 = BitConverter.ToInt32(pkt.payload, 0);
                        int t2 = BitConverter.ToInt32(pkt.payload, 4);
                        state.myTimeMs = t1;
                        state.opponentTimeMs = t2;
                        OnStateUpdate?.Invoke(state);
                    }
                    break;

                case MsgType.YOUR_TURN:
                    state.myTurn = true;
                    OnYourTurn?.Invoke();
                    break;

                case MsgType.GAME_OVER:
                    if (pkt.payload.Length >= 1)
                    {
                        state.gameOver = true;
                        state.winnerId = pkt.payload[0];
                        OnGameOver?.Invoke(state.winnerId);
                    }
                    break;

                case MsgType.ERROR_MSG:
                    string errMsg = Encoding.ASCII.GetString(pkt.payload);
                    Debug.LogWarning("[NetworkManager] Server error message: " + errMsg);
                    break;

                case MsgType.PONG:
                    // Log keep-alive response
                    break;
            }
        }
    }
}
