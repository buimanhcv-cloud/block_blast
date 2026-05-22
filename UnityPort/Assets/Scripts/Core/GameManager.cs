using System.Collections;
using UnityEngine;

namespace BlockBlast.Core
{
    using BlockBlast.UI;
    using BlockBlast.Network;

    public enum GameState
    {
        MENU,
        PLAYING_SOLO,
        PLAYING_VS_AI,
        GAME_OVER,
        ONLINE_SELECT,
        ONLINE_PLAYING
    }

    public class GameManager : MonoBehaviour
    {
        public const int TRAY_SIZE = 3;

        [Header("References")]
        public Board board;
        public GameObject blockPrefab;
        public Transform[] traySlots;

        [Header("UI Controllers")]
        public MenuController menuController;
        public HUDController hudController;

        [Header("Audio")]
        public AudioSource audioSource;
        public AudioClip placeSfx;
        public AudioClip clearSfx;
        public AudioClip comboSfx;
        public AudioClip gameOverSfx;

        [Header("AI Configuration")]
        public float aiThinkDelay = 1.2f;

        private GameState currentState = GameState.MENU;
        private Block[] trayBlocks = new Block[TRAY_SIZE];
        private Block ghostBlockInstance;

        private int score = 0;
        private int highScore = 0;
        private int combo = 0;
        private bool aiWaiting = false;

        public GameState CurrentState => currentState;
        public int Score => score;
        public int HighScore => highScore;

        private void Start()
        {
            highScore = PlayerPrefs.GetInt("HighScore", 0);
            currentState = GameState.MENU;
            if (menuController != null) menuController.ShowMainMenu();
            if (hudController != null) hudController.gameObject.SetActive(false);

            // Instantiate ghost preview block representation
            if (blockPrefab != null)
            {
                GameObject ghostObj = Instantiate(blockPrefab, transform);
                ghostBlockInstance = ghostObj.GetComponent<Block>();
                ghostBlockInstance.gameObject.SetActive(false);

                // Disable colliders and interaction on the ghost block
                var colliders = ghostBlockInstance.GetComponentsInChildren<Collider2D>();
                foreach (var c in colliders) c.enabled = false;
                var dragger = ghostBlockInstance.GetComponent<BlockDragger>();
                if (dragger != null) Destroy(dragger);
            }
        }

        public void StartSolo()
        {
            currentState = GameState.PLAYING_SOLO;
            score = 0;
            combo = 0;
            board.ResetBoard();
            ClearTray();
            RefillTray();
            
            if (hudController != null)
            {
                hudController.gameObject.SetActive(true);
                hudController.UpdateScore(score, highScore);
            }
            if (menuController != null) menuController.HideAllScreens();
        }

        public void StartVsAI()
        {
            currentState = GameState.PLAYING_VS_AI;
            score = 0;
            combo = 0;
            aiWaiting = false;
            board.ResetBoard();
            ClearTray();
            RefillTray();

            if (hudController != null)
            {
                hudController.gameObject.SetActive(true);
                hudController.UpdateScore(score, highScore);
            }
            if (menuController != null) menuController.HideAllScreens();
        }

        public void EndGame()
        {
            if (score > highScore)
            {
                highScore = score;
                PlayerPrefs.SetInt("HighScore", highScore);
                PlayerPrefs.Save();
            }
            currentState = GameState.GAME_OVER;
            PlaySound(gameOverSfx);

            if (menuController != null)
            {
                menuController.ShowGameOverScreen(score, highScore);
            }
        }

        public bool CanInteract(Block block)
        {
            if (currentState == GameState.PLAYING_VS_AI && aiWaiting) return false;
            return currentState == GameState.PLAYING_SOLO || currentState == GameState.PLAYING_VS_AI || currentState == GameState.ONLINE_PLAYING;
        }

        private void RefillTray()
        {
            for (int i = 0; i < TRAY_SIZE; i++)
            {
                if (blockPrefab != null && traySlots[i] != null)
                {
                    GameObject blockObj = Instantiate(blockPrefab, traySlots[i].position, Quaternion.identity);
                    Block block = blockObj.GetComponent<Block>();
                    block.InitializeRandom();
                    block.SetPreviewMode(true);

                    BlockDragger dragger = blockObj.GetComponent<BlockDragger>();
                    if (dragger != null)
                    {
                        dragger.SetOriginalPosition(traySlots[i].position);
                    }

                    trayBlocks[i] = block;
                }
            }
        }

        private void ClearTray()
        {
            for (int i = 0; i < TRAY_SIZE; i++)
            {
                if (trayBlocks[i] != null)
                {
                    Destroy(trayBlocks[i].gameObject);
                    trayBlocks[i] = null;
                }
            }
        }

        public void OnBlockDragStart(Block block)
        {
            if (ghostBlockInstance != null)
            {
                // Sync ghost block shape
                ghostBlockInstance.InitializeCustom(block.Shape, new Color(block.BlockColor.r, block.BlockColor.g, block.BlockColor.b, 0.4f));
            }
        }

        public void OnBlockDragUpdate(Block block)
        {
            if (ghostBlockInstance == null) return;

            // In C++, the dragging position is centered or top-left.
            // Let's use board conversion on block's position (which is centered in world space).
            // But shape positions are offsets relative to pivot. 
            // In Board.cs, CanPlaceBlock takes the top-left index pos. 
            // Since our block visual is centered in Unity, we calculate the offset.
            Vector2Int boardPos = GetTopLeftBoardPosOfBlock(block);

            if (board.CanPlaceBlock(block.Shape, boardPos))
            {
                // Draw ghost
                ghostBlockInstance.gameObject.SetActive(true);
                // Position of ghost should be snapped to grid matching the boardPos
                ghostBlockInstance.transform.position = GetWorldSnappedPosition(block, boardPos);
            }
            else
            {
                ghostBlockInstance.gameObject.SetActive(false);
            }
        }

        public bool OnBlockDragEnd(Block block)
        {
            if (ghostBlockInstance != null)
            {
                ghostBlockInstance.gameObject.SetActive(false);
            }

            Vector2Int boardPos = GetTopLeftBoardPosOfBlock(block);

            if (board.CanPlaceBlock(block.Shape, boardPos))
            {
                board.PlaceBlock(block.Shape, boardPos);
                PlaySound(placeSfx);
                CheckForClears();

                // Remove from tray
                for (int i = 0; i < TRAY_SIZE; i++)
                {
                    if (trayBlocks[i] == block)
                    {
                        trayBlocks[i] = null;
                        Destroy(block.gameObject);
                        break;
                    }
                }

                // If tray empty, refill
                bool allEmpty = true;
                for (int i = 0; i < TRAY_SIZE; i++)
                {
                    if (trayBlocks[i] != null) allEmpty = false;
                }
                if (allEmpty) RefillTray();

                // Check end game conditions
                if (!AnyBlockCanBePlaced())
                {
                    EndGame();
                }
                else if (currentState == GameState.PLAYING_VS_AI)
                {
                    aiWaiting = true;
                    StartCoroutine(ExecuteAIMoveRoutine());
                }

                return true;
            }

            return false;
        }

        private IEnumerator ExecuteAIMoveRoutine()
        {
            yield return new WaitForSeconds(aiThinkDelay);

            AIPlayer.Move move = AIPlayer.FindBestMove(board, trayBlocks);
            if (!move.valid)
            {
                EndGame();
                yield break;
            }

            Block aiBlock = trayBlocks[move.trayIndex];
            board.PlaceBlock(aiBlock.Shape, move.boardPos);
            PlaySound(placeSfx);
            CheckForClears();

            // Destroy visual and clear tray slot
            trayBlocks[move.trayIndex] = null;
            Destroy(aiBlock.gameObject);

            // Refill tray if empty
            bool allEmpty = true;
            for (int i = 0; i < TRAY_SIZE; i++)
            {
                if (trayBlocks[i] != null) allEmpty = false;
            }
            if (allEmpty) RefillTray();

            // Check game over
            if (!AnyBlockCanBePlaced())
            {
                EndGame();
            }

            aiWaiting = false;
        }

        private void CheckForClears()
        {
            int lines = board.ClearFullLines();
            if (lines > 0)
            {
                score += lines * 10 * (combo + 1);
                combo++;
                PlaySound(clearSfx);
                if (combo > 1) PlaySound(comboSfx);

                if (hudController != null)
                {
                    hudController.UpdateScore(score, highScore);
                }
            }
            else
            {
                combo = 0;
            }
        }

        private bool AnyBlockCanBePlaced()
        {
            for (int i = 0; i < TRAY_SIZE; i++)
            {
                if (trayBlocks[i] == null) continue;
                if (board.CanPlaceAnyBlock(trayBlocks[i].Shape)) return true;
            }
            return false;
        }

        private Vector2Int GetTopLeftBoardPosOfBlock(Block block)
        {
            int rows = block.Shape.GetLength(0);
            int cols = block.Shape.GetLength(1);

            // Center of block is block.transform.position.
            // Top-left cell pivot in world space:
            float halfWidth = (cols * block.cellSize) / 2f;
            float halfHeight = (rows * block.cellSize) / 2f;

            // In world space, the top-left visual cell center position is:
            Vector3 topLeftCellWorld = block.transform.position + new Vector3(
                -halfWidth + block.cellSize / 2f,
                halfHeight - block.cellSize / 2f,
                0f
            );

            // Convert that cell to grid coordinate index
            return board.GetBoardPosition(topLeftCellWorld);
        }

        private Vector3 GetWorldSnappedPosition(Block block, Vector2Int boardPos)
        {
            int rows = block.Shape.GetLength(0);
            int cols = block.Shape.GetLength(1);

            float halfWidth = (cols * block.cellSize) / 2f;
            float halfHeight = (rows * block.cellSize) / 2f;

            // Snap the top-left cell center of the block to the board cell coordinate world position
            Vector3 cellWorldPos = board.GetCellWorldPosition(boardPos.x, boardPos.y);

            // Find the center of the block asset
            return cellWorldPos - new Vector3(
                -halfWidth + block.cellSize / 2f,
                halfHeight - block.cellSize / 2f,
                0f
            );
        }

        private void PlaySound(AudioClip clip)
        {
            if (audioSource != null && clip != null)
            {
                audioSource.PlayOneShot(clip);
            }
        }
    }
}
