using UnityEngine;
using UnityEngine.UI;

namespace BlockBlast.UI
{
    public class HUDController : MonoBehaviour
    {
        [Header("UI References")]
        public Text scoreText;
        public Text bestScoreText;
        public Text comboText;

        public void UpdateScore(int score, int highScore)
        {
            if (scoreText != null)
            {
                scoreText.text = "SCORE: " + score;
            }

            if (bestScoreText != null)
            {
                bestScoreText.text = "BEST: " + highScore;
            }
        }

        public void UpdateCombo(int combo)
        {
            if (comboText != null)
            {
                if (combo > 0)
                {
                    comboText.gameObject.SetActive(true);
                    comboText.text = "COMBO x" + combo;
                }
                else
                {
                    comboText.gameObject.SetActive(false);
                }
            }
        }
    }
}
