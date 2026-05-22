using System;
using System.Collections.Concurrent;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using UnityEngine;

namespace BlockBlast.Network
{
    public enum MsgType : byte
    {
        // Client -> Server
        JOIN         = 0x01,
        MOVE         = 0x02,
        READY        = 0x03,
        PING         = 0x04,

        // Server -> Client
        ROOM_JOINED  = 0x10,
        GAME_START   = 0x11,
        GAME_STATE   = 0x12,
        SCORE_UPDATE = 0x13,
        TIME_UPDATE  = 0x14,
        YOUR_TURN    = 0x15,
        GAME_OVER    = 0x16,
        PONG         = 0x17,
        ERROR_MSG    = 0x18
    }

    public struct NetPacket
    {
        public MsgType type;
        public byte[] payload;
    }

    public class NetworkClient
    {
        private TcpClient socket;
        private NetworkStream stream;
        private CancellationTokenSource cts;

        private readonly ConcurrentQueue<NetPacket> inQueue = new ConcurrentQueue<NetPacket>();
        private readonly ConcurrentQueue<byte[]> outQueue = new ConcurrentQueue<byte[]>();

        private bool connected = false;
        private bool isSending = false;

        public bool IsConnected => connected && socket != null && socket.Connected;

        public async Task<bool> ConnectAsync(string host, int port)
        {
            Disconnect();

            try
            {
                socket = new TcpClient();
                cts = new CancellationTokenSource();

                // Connect with timeout
                var connectTask = socket.ConnectAsync(host, port);
                var timeoutTask = Task.Delay(5000, cts.Token);

                var completedTask = await Task.WhenAny(connectTask, timeoutTask);
                if (completedTask == timeoutTask || !socket.Connected)
                {
                    socket.Close();
                    return false;
                }

                stream = socket.GetStream();
                connected = true;

                // Start receive loop
                _ = Task.Run(() => ReceiveLoop(cts.Token));

                return true;
            }
            catch (Exception ex)
            {
                Debug.LogError("[NetworkClient] Connection error: " + ex.Message);
                Disconnect();
                return false;
            }
        }

        public void Disconnect()
        {
            connected = false;
            cts?.Cancel();

            if (stream != null)
            {
                stream.Close();
                stream = null;
            }

            if (socket != null)
            {
                socket.Close();
                socket = null;
            }

            // Flush queues
            while (inQueue.TryDequeue(out _)) { }
            while (outQueue.TryDequeue(out _)) { }
        }

        public bool PollPacket(out NetPacket packet)
        {
            return inQueue.TryDequeue(out packet);
        }

        // --- Send Helpers ---

        public void SendJoin(string roomId, int mode, int timeLimitSec)
        {
            byte[] roomBytes = Encoding.ASCII.GetBytes(roomId);
            byte[] payload = new byte[3 + roomBytes.Length];
            payload[0] = (byte)mode;
            payload[1] = (byte)timeLimitSec;
            payload[2] = (byte)roomBytes.Length;
            Array.Copy(roomBytes, 0, payload, 3, roomBytes.Length);

            EnqueuePacket(MsgType.JOIN, payload);
        }

        public void SendMove(int boardX, int boardY, int[,] shape)
        {
            int rows = shape.GetLength(0);
            int cols = shape.GetLength(1);

            byte[] payload = new byte[4 + rows * cols];
            payload[0] = (byte)boardX;
            payload[1] = (byte)boardY;
            payload[2] = (byte)rows;
            payload[3] = (byte)cols;

            int idx = 4;
            for (int r = 0; r < rows; r++)
            {
                for (int c = 0; c < cols; c++)
                {
                    payload[idx++] = (byte)shape[r, c];
                }
            }

            EnqueuePacket(MsgType.MOVE, payload);
        }

        public void SendReady()
        {
            EnqueuePacket(MsgType.READY, new byte[0]);
        }

        public void SendPing()
        {
            EnqueuePacket(MsgType.PING, new byte[0]);
        }

        // --- Low-level Socket Operations ---

        private void EnqueuePacket(MsgType type, byte[] payload)
        {
            if (!IsConnected) return;

            // Wire format: [1 byte type][2 bytes payload length LE][N bytes payload]
            byte[] packetBytes = new byte[3 + payload.Length];
            packetBytes[0] = (byte)type;
            packetBytes[1] = (byte)(payload.Length & 0xFF);
            packetBytes[2] = (byte)((payload.Length >> 8) & 0xFF);
            Array.Copy(payload, 0, packetBytes, 3, payload.Length);

            outQueue.Enqueue(packetBytes);
            TriggerSend();
        }

        private async void TriggerSend()
        {
            if (isSending || !IsConnected) return;
            isSending = true;

            try
            {
                while (outQueue.TryDequeue(out byte[] data))
                {
                    await stream.WriteAsync(data, 0, data.Length, cts.Token);
                    await stream.FlushAsync(cts.Token);
                }
            }
            catch (Exception ex)
            {
                Debug.LogError("[NetworkClient] Write error: " + ex.Message);
                Disconnect();
            }
            finally
            {
                isSending = false;
            }
        }

        private async Task ReceiveLoop(CancellationToken token)
        {
            byte[] header = new byte[3];

            try
            {
                while (!token.IsCancellationRequested && IsConnected)
                {
                    // Read header: [1 byte type][2 bytes length]
                    if (!await ReadExactAsync(header, 3, token)) break;

                    MsgType type = (MsgType)header[0];
                    ushort payloadLen = (ushort)(header[1] | (header[2] << 8));

                    byte[] payload = new byte[payloadLen];
                    if (payloadLen > 0)
                    {
                        if (!await ReadExactAsync(payload, payloadLen, token)) break;
                    }

                    inQueue.Enqueue(new NetPacket { type = type, payload = payload });
                }
            }
            catch (Exception ex)
            {
                if (!token.IsCancellationRequested)
                {
                    Debug.LogError("[NetworkClient] Read error: " + ex.Message);
                }
            }
            finally
            {
                Disconnect();
            }
        }

        private async Task<bool> ReadExactAsync(byte[] buffer, int count, CancellationToken token)
        {
            int totalRead = 0;
            while (totalRead < count)
            {
                int read = await stream.ReadAsync(buffer, totalRead, count - totalRead, token);
                if (read <= 0) return false; // Connection closed
                totalRead += read;
            }
            return true;
        }
    }
}
