using System;
using UnityEngine;
using UnityEngine.UI;

namespace BlockBlast.UI
{
    using BlockBlast.Core;
    using BlockBlast.Network;

    public class OnlineGameMode : MonoBehaviour
    {
        [Header("References")]
        public GameNetworkManager netManager;
        public GameManager gameManager;
        public Board localBoard;

        [Header("UI Panels & Screens")]
        public GameObject waitingOverlay;
        public Text statusText;
        public Text scoreComparisonText;
        public Text myTimerText;
        public Text opponentTimerText;
        public Text turnIndicatorText;

        [Header("Opponent Board Visuals")]
        public GameObject opponentCellPrefab;
        public Transform opponentGridRoot;
        public float opponentCellSize = 0.5f;

        private GameObject[,] opponentCellInstances = new GameObject[Board.SIZE, Board.SIZE];
        private bool isGameStarted = false;

        private void Start()
        {
            if (netManager != null)
            {
                netManager.OnRoomJoined += HandleRoomJoined;
                netManager.OnGameStart += HandleGameStart;
                netManager.OnStateUpdate += HandleStateUpdate;
                netManager.OnYourTurn += HandleYourTurn;
                netManager.OnGameOver += HandleGameOver;
                netManager.OnDisconnected += HandleDisconnected;
            }

            BuildOpponentGridVisuals();
            gameObject.SetActive(false);
        }

        private void OnDestroy()
        {
            if (netManager != null)
            {
                netManager.OnRoomJoined -= HandleRoomJoined;
                netManager.OnGameStart -= HandleGameStart;
                netManager.OnStateUpdate -= HandleStateUpdate;
                netManager.OnYourTurn -= HandleYourTurn;
                netManager.OnGameOver -= HandleGameOver;
                netManager.OnDisconnected -= HandleDisconnected;
            }
        }

        private void BuildOpponentGridVisuals()
        {
            if (opponentCellPrefab == null || opponentGridRoot == null) return;

            // Clear previous cells
            foreach (Transform t in opponentGridRoot)
            {
                Destroy(t.gameObject);
            }

            // Create 8x8 small cells representing the opponent's grid
            float startOffset = -((Board.SIZE * opponentCellSize) / 2f) + opponentCellSize / 2f;

            for (int y = 0; y < Board.SIZE; y++)
            {
                for (int x = 0; x < Board.SIZE; x++)
                {
                    GameObject cell = Instantiate(opponentCellPrefab, opponentGridRoot);
                    // Col-major alignment in 2D Space
                    cell.transform.localPosition = new Vector3(
                        startOffset + x * opponentCellSize,
                        startOffset + (Board.SIZE - 1 - y) * opponentCellSize,
                        0f
                    );

                    opponentCellInstances[x, y] = cell;
                    UpdateCellVisual(x, y, 0); // Default empty
                }
            }
        }

        private void UpdateCellVisual(int x, int y, int filled)
        {
            if (opponentCellInstances[x, y] == null) return;
            var renderer = opponentCellInstances[x, y].GetComponentInChildren<SpriteRenderer>();
            if (renderer != null)
            {
                // Filled is cyan/blue, empty is dark purple
                renderer.color = (filled == 1) ? new Color(0.36f, 0.83f, 1f) : new Color(0.2f, 0.12f, 0.22f);
            }
        }

        // --- Network Callback Handlers ---

        private void HandleRoomJoined(int myPlayerId)
        {
            gameObject.SetActive(true);
            isGameStarted = false;
            if (waitingOverlay != null) waitingOverlay.SetActive(true);
            if (statusText != null) statusText.text = $"Room joined. Waiting for opponent (ID: {myPlayerId + 1})...";
        }

        private void HandleGameStart(OnlineMode mode, int timeSec)
        {
            isGameStarted = true;
            if (waitingOverlay != null) waitingOverlay.SetActive(false);
            if (statusText != null) statusText.text = $"Game started: {mode}";

            // Reset local game state to start playing
            if (gameManager != null)
            {
                gameManager.StartSolo(); // Initialize local board & tray
            }
        }

        private void HandleStateUpdate(OnlineGameState state)
        {
            if (!isGameStarted) return;

            // Score HUD update
            if (scoreComparisonText != null)
            {
                scoreComparisonText.text = $"Score: {state.myScore} vs Opponent: {state.opponentScore}";
            }

            // Timers HUD update
            if (myTimerText != null)
            {
                myTimerText.text = $"My Time: {state.myTimeMs / 1000f:F1}s";
            }
            if (opponentTimerText != null)
            {
                opponentTimerText.text = $"Opp Time: {state.opponentTimeMs / 1000f:F1}s";
            }

            // Turn Indicator
            if (turnIndicatorText != null)
            {
                turnIndicatorText.text = state.myTurn ? "YOUR TURN!" : "Opponent Turn...";
            }

            // Update opponent grid visuals
            for (int y = 0; y < Board.SIZE; y++)
            {
                for (int x = 0; x < Board.SIZE; x++)
                {
                    UpdateCellVisual(x, y, state.opponentGrid[x, y]);
                }
            }
        }

        private void HandleYourTurn()
        {
            if (turnIndicatorText != null)
            {
                turnIndicatorText.text = "YOUR TURN!";
            }
        }

        private void HandleGameOver(int winnerId)
        {
            isGameStarted = false;
            bool iWon = (winnerId == netManager.State.myPlayerId);
            if (statusText != null)
            {
                statusText.text = iWon ? "Victory!" : "Defeat!";
            }
            if (gameManager != null)
            {
                gameManager.EndGame();
            }
        }

        private void HandleDisconnected()
        {
            isGameStarted = false;
            gameObject.SetActive(false);
            if (gameManager != null && gameManager.menuController != null)
            {
                gameManager.menuController.ShowMainMenu();
            }
        }
    }
}
