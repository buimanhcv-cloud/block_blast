using UnityEngine;
using UnityEngine.UI;

namespace BlockBlast.UI
{
    using BlockBlast.Core;

    public class MenuController : MonoBehaviour
    {
        [Header("Panels")]
        public GameObject mainMenuPanel;
        public GameObject gameOverPanel;
        public GameObject onlineSelectPanel;

        [Header("Game Over Fields")]
        public Text gameOverScoreText;
        public Text gameOverBestText;
        public GameObject newBestLabel;

        private GameManager gameManager;

        private void Awake()
        {
            gameManager = FindObjectOfType<GameManager>();
        }

        public void ShowMainMenu()
        {
            HideAllScreens();
            if (mainMenuPanel != null) mainMenuPanel.SetActive(true);
        }

        public void ShowGameOverScreen(int score, int highScore)
        {
            HideAllScreens();
            if (gameOverPanel != null)
            {
                gameOverPanel.SetActive(true);
                if (gameOverScoreText != null) gameOverScoreText.text = "SCORE: " + score;
                if (gameOverBestText != null) gameOverBestText.text = "BEST: " + highScore;
                if (newBestLabel != null) newBestLabel.SetActive(score >= highScore && score > 0);
            }
        }

        public void ShowOnlineSelectScreen()
        {
            HideAllScreens();
            if (onlineSelectPanel != null) onlineSelectPanel.SetActive(true);
        }

        public void HideAllScreens()
        {
            if (mainMenuPanel != null) mainMenuPanel.SetActive(false);
            if (gameOverPanel != null) gameOverPanel.SetActive(false);
            if (onlineSelectPanel != null) onlineSelectPanel.SetActive(false);
        }

        // --- UI Button Listeners ---

        public void OnSoloClick()
        {
            if (gameManager != null)
            {
                gameManager.StartSolo();
            }
        }

        public void OnVsAIClick()
        {
            if (gameManager != null)
            {
                gameManager.StartVsAI();
            }
        }

        public void OnOnlineClick()
        {
            ShowOnlineSelectScreen();
        }

        public void OnQuitClick()
        {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
            Application.Quit();
#endif
        }

        public void OnRestartClick()
        {
            if (gameManager != null)
            {
                if (gameManager.CurrentState == GameState.GAME_OVER)
                {
                    // Restart whatever mode was running. Default to Solo.
                    gameManager.StartSolo();
                }
            }
        }

        public void OnBackToMenuClick()
        {
            ShowMainMenu();
            if (gameManager != null && gameManager.hudController != null)
            {
                gameManager.hudController.gameObject.SetActive(false);
            }
        }
    }
}
