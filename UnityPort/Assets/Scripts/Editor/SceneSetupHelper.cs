#if UNITY_EDITOR
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.UI;

namespace BlockBlast.Editor
{
    using BlockBlast.Core;
    using BlockBlast.UI;
    using BlockBlast.Network;

    public class SceneSetupHelper : EditorWindow
    {
        [MenuItem("Tools/Setup Block Blast Scene")]
        public static void SetupScene()
        {
            // 1. Create a new 2D Scene
            var scene = EditorSceneManager.NewScene(NewSceneSetup.DefaultGameObjects, NewSceneMode.Single);
            
            // Remove the default Main Camera to configure our own
            GameObject defaultCam = GameObject.Find("Main Camera");
            if (defaultCam != null) DestroyImmediate(defaultCam);

            // 2. Create Custom Main Camera
            GameObject camObj = new GameObject("Main Camera");
            Camera camera = camObj.AddComponent<Camera>();
            camera.orthographic = true;
            camera.orthographicSize = 5f;
            camObj.transform.position = new Vector3(0, 0, -10);

            // 3. Create Board Game Object
            GameObject boardObj = new GameObject("Board");
            Board board = boardObj.AddComponent<Board>();
            board.cellSize = 0.8f;
            board.gridOffset = new Vector2(-2.8f, -1.0f); // Center the 8x8 grid on screen

            // 4. Create UI Canvas
            GameObject canvasObj = new GameObject("Canvas");
            Canvas canvas = canvasObj.AddComponent<Canvas>();
            canvas.renderMode = RenderMode.ScreenSpaceOverlay;
            canvasObj.AddComponent<CanvasScaler>();
            canvasObj.AddComponent<GraphicRaycaster>();

            // Create Event System if not present
            if (GameObject.FindObjectOfType<UnityEngine.EventSystems.EventSystem>() == null)
            {
                GameObject eventSystem = new GameObject("EventSystem");
                eventSystem.AddComponent<UnityEngine.EventSystems.EventSystem>();
                eventSystem.AddComponent<UnityEngine.EventSystems.StandaloneInputModule>();
            }

            // Create Menu Controller Component
            MenuController menuController = canvasObj.AddComponent<MenuController>();

            // 5. Create HUD Panel (In-game overlays)
            GameObject hudPanel = CreatePanel(canvasObj, "HUDPanel");
            HUDController hudController = hudPanel.AddComponent<HUDController>();
            
            Text scoreText = CreateText(hudPanel, "ScoreText", "SCORE: 0", 18, new Vector2(-150, 200), new Vector2(150, 40));
            Text bestText = CreateText(hudPanel, "BestScoreText", "BEST: 0", 18, new Vector2(150, 200), new Vector2(150, 40));
            Text comboText = CreateText(hudPanel, "ComboText", "COMBO x0", 16, new Vector2(0, 150), new Vector2(150, 40));
            Text turnText = CreateText(hudPanel, "TurnIndicatorText", "", 16, new Vector2(0, 110), new Vector2(250, 40));
            comboText.alignment = TextAnchor.MiddleCenter;
            turnText.alignment = TextAnchor.MiddleCenter;
            turnText.color = Color.yellow;

            hudController.scoreText = scoreText;
            hudController.bestScoreText = bestText;
            hudController.comboText = comboText;

            // 6. Create Main Menu Panel
            GameObject menuPanel = CreatePanel(canvasObj, "MainMenuPanel");
            CreateText(menuPanel, "TitleText", "BLOCK BLAST", 36, new Vector2(0, 150), new Vector2(300, 60), TextAnchor.MiddleCenter);
            
            Button soloBtn = CreateButton(menuPanel, "SoloButton", "SOLO PLAY", new Vector2(0, 40));
            Button vsAiBtn = CreateButton(menuPanel, "VsAIButton", "VS COMPUTER", new Vector2(0, -20));
            Button onlineBtn = CreateButton(menuPanel, "OnlineButton", "MULTIPLAYER", new Vector2(0, -80));
            Button quitBtn = CreateButton(menuPanel, "QuitButton", "QUIT GAME", new Vector2(0, -140));

            // Assign Main Menu Listeners to MenuController
            soloBtn.onClick.AddListener(menuController.OnSoloClick);
            vsAiBtn.onClick.AddListener(menuController.OnVsAIClick);
            onlineBtn.onClick.AddListener(menuController.OnOnlineClick);
            quitBtn.onClick.AddListener(menuController.OnQuitClick);

            // 7. Create Game Over Panel
            GameObject gameOverPanel = CreatePanel(canvasObj, "GameOverPanel");
            CreateText(gameOverPanel, "GameOverTitle", "GAME OVER", 32, new Vector2(0, 120), new Vector2(300, 50), TextAnchor.MiddleCenter);
            Text goScoreText = CreateText(gameOverPanel, "GoScoreText", "SCORE: 0", 22, new Vector2(0, 60), new Vector2(200, 40), TextAnchor.MiddleCenter);
            Text goBestText = CreateText(gameOverPanel, "GoBestText", "BEST: 0", 22, new Vector2(0, 10), new Vector2(200, 40), TextAnchor.MiddleCenter);
            GameObject newBestLabel = CreateText(gameOverPanel, "NewBestText", "NEW BEST!", 20, new Vector2(0, -30), new Vector2(200, 45), TextAnchor.MiddleCenter).gameObject;
            newBestLabel.GetComponent<Text>().color = Color.green;

            Button restartBtn = CreateButton(gameOverPanel, "RestartButton", "PLAY AGAIN", new Vector2(0, -100));
            Button backBtn = CreateButton(gameOverPanel, "BackButton", "MAIN MENU", new Vector2(0, -160));

            restartBtn.onClick.AddListener(menuController.OnRestartClick);
            backBtn.onClick.AddListener(menuController.OnBackToMenuClick);

            menuController.mainMenuPanel = menuPanel;
            menuController.gameOverPanel = gameOverPanel;
            menuController.gameOverScoreText = goScoreText;
            menuController.gameOverBestText = goBestText;
            menuController.newBestLabel = newBestLabel;

            // 8. Create Online Room Lobby Panel
            GameObject onlinePanel = CreatePanel(canvasObj, "OnlineSelectPanel");
            onlinePanel.AddComponent<Image>().color = new Color(0.12f, 0.08f, 0.18f, 0.95f);
            
            CreateText(onlinePanel, "LobbyTitle", "MULTIPLAYER ROOM", 26, new Vector2(0, 180), new Vector2(300, 40), TextAnchor.MiddleCenter);
            
            InputField hostInput = CreateInputField(onlinePanel, "HostInput", "Host Address (e.g. 127.0.0.1)", new Vector2(0, 110));
            InputField portInput = CreateInputField(onlinePanel, "PortInput", "Port (e.g. 5000)", new Vector2(0, 50));
            InputField roomInput = CreateInputField(onlinePanel, "RoomInput", "Room ID", new Vector2(0, -10));

            Text statusText = CreateText(onlinePanel, "StatusText", "Enter details and connect", 14, new Vector2(0, -70), new Vector2(300, 30), TextAnchor.MiddleCenter);
            
            Button lobbyJoinBtn = CreateButton(onlinePanel, "LobbyJoinButton", "CONNECT & JOIN", new Vector2(0, -120));
            Button lobbyBackBtn = CreateButton(onlinePanel, "LobbyBackButton", "BACK", new Vector2(0, -180));

            menuController.onlineSelectPanel = onlinePanel;

            // 9. Create Network Manager and Online Mode
            GameObject netManagerObj = new GameObject("GameNetworkManager");
            GameNetworkManager netManager = netManagerObj.AddComponent<GameNetworkManager>();

            // Online select UI binder
            OnlineGameSelectController lobbySelectController = onlinePanel.AddComponent<OnlineGameSelectController>();
            lobbySelectController.hostInput = hostInput;
            lobbySelectController.portInput = portInput;
            lobbySelectController.roomIdInput = roomInput;
            lobbySelectController.statusText = statusText;
            lobbySelectController.connectButton = lobbyJoinBtn;
            lobbySelectController.backButton = lobbyBackBtn;
            lobbySelectController.netManager = netManager;
            lobbySelectController.menuController = menuController;

            // Online gameplay visual binder
            GameObject onlineGameObj = new GameObject("OnlineGameModeView");
            OnlineGameMode onlineGameMode = onlineGameObj.AddComponent<OnlineGameMode>();
            onlineGameMode.netManager = netManager;
            onlineGameMode.localBoard = board;
            onlineGameMode.statusText = statusText;
            onlineGameMode.scoreComparisonText = scoreText; // Share same in-game Texts
            onlineGameMode.myTimerText = CreateText(hudPanel, "MyTimerText", "My Time: --", 14, new Vector2(-150, 160), new Vector2(150, 30));
            onlineGameMode.opponentTimerText = CreateText(hudPanel, "OpponentTimerText", "Opp Time: --", 14, new Vector2(150, 160), new Vector2(150, 30));
            onlineGameMode.turnIndicatorText = turnText;

            // Create mini board placeholder for opponent
            GameObject oppBoardRoot = new GameObject("OpponentBoardVisuals");
            oppBoardRoot.transform.SetParent(onlineGameObj.transform);
            oppBoardRoot.transform.position = new Vector3(4.5f, 1.0f, 0f); // Offset to the right
            onlineGameMode.opponentGridRoot = oppBoardRoot.transform;
            
            // Create dummy cell prefab for opponent board visual rendering
            GameObject oppCellDummy = new GameObject("OpponentCellPrefab");
            SpriteRenderer spr = oppCellDummy.AddComponent<SpriteRenderer>();
            spr.sprite = Sprite.Create(Texture2D.whiteTexture, new Rect(0, 0, 4, 4), new Vector2(0.5f, 0.5f));
            oppCellDummy.transform.SetParent(onlineGameObj.transform);
            oppCellDummy.SetActive(false);
            onlineGameMode.opponentCellPrefab = oppCellDummy;

            // 10. Create GameManager Game Object
            GameObject gmObj = new GameObject("GameManager");
            GameManager gameManager = gmObj.AddComponent<GameManager>();
            gameManager.board = board;
            gameManager.menuController = menuController;
            gameManager.hudController = hudController;
            gameManager.audioSource = gmObj.AddComponent<AudioSource>();

            // Setup tray slot transforms
            gameManager.traySlots = new Transform[GameManager.TRAY_SIZE];
            for (int i = 0; i < GameManager.TRAY_SIZE; i++)
            {
                GameObject slot = new GameObject($"TraySlot_{i + 1}");
                slot.transform.SetParent(gmObj.transform);
                // Align slots below the board
                slot.transform.position = new Vector3(-2f + i * 2f, -3.5f, 0f);
                gameManager.traySlots[i] = slot.transform;
            }

            onlineGameMode.gameManager = gameManager;

            // 11. Create a dummy Cell Prefab for Block representation
            GameObject cellPrefab = new GameObject("CellPrefab");
            SpriteRenderer cellSpr = cellPrefab.AddComponent<SpriteRenderer>();
            cellSpr.sprite = Sprite.Create(Texture2D.whiteTexture, new Rect(0, 0, 1, 1), new Vector2(0.5f, 0.5f));
            cellPrefab.AddComponent<BoxCollider2D>();
            cellPrefab.transform.SetParent(gmObj.transform);
            cellPrefab.SetActive(false);

            // Create a Block Prefab and attach Block.cs + BlockDragger.cs
            GameObject blockPrefab = new GameObject("BlockPrefab");
            Block blockComponent = blockPrefab.AddComponent<Block>();
            blockComponent.cellPrefab = cellPrefab;
            blockPrefab.AddComponent<BlockDragger>();
            BoxCollider2D blockCollider = blockPrefab.AddComponent<BoxCollider2D>();
            blockCollider.isTrigger = true;
            blockCollider.size = new Vector2(1f, 1f);
            blockPrefab.transform.SetParent(gmObj.transform);
            blockPrefab.SetActive(false);

            gameManager.blockPrefab = blockPrefab;

            // 12. Save scene to assets
            if (!AssetDatabase.IsValidFolder("Assets/Scenes"))
            {
                AssetDatabase.CreateFolder("Assets", "Scenes");
            }

            EditorSceneManager.SaveScene(scene, "Assets/Scenes/Game.unity");
            Debug.Log("[Setup] Block Blast Unity Scene created and saved to Assets/Scenes/Game.unity successfully!");
        }

        // --- UI Construction Helpers ---

        private static GameObject CreatePanel(GameObject parent, string name)
        {
            GameObject panel = new GameObject(name);
            RectTransform rect = panel.AddComponent<RectTransform>();
            panel.transform.SetParent(parent.transform, false);
            rect.anchorMin = Vector2.zero;
            rect.anchorMax = Vector2.one;
            rect.sizeDelta = Vector2.zero;
            return panel;
        }

        private static Text CreateText(GameObject parent, string name, string content, int fontSize, Vector2 position, Vector2 size, TextAnchor anchor = TextAnchor.UpperLeft)
        {
            GameObject textObj = new GameObject(name);
            textObj.transform.SetParent(parent.transform, false);
            Text text = textObj.AddComponent<Text>();
            text.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
            text.text = content;
            text.fontSize = fontSize;
            text.alignment = anchor;
            text.color = Color.white;

            RectTransform rect = textObj.GetComponent<RectTransform>();
            rect.anchoredPosition = position;
            rect.sizeDelta = size;
            return text;
        }

        private static Button CreateButton(GameObject parent, string name, string labelText, Vector2 position)
        {
            GameObject btnObj = new GameObject(name);
            btnObj.transform.SetParent(parent.transform, false);
            
            // Image component for background
            Image img = btnObj.AddComponent<Image>();
            img.color = new Color(0.25f, 0.15f, 0.28f);

            Button btn = btnObj.AddComponent<Button>();
            RectTransform rect = btnObj.GetComponent<RectTransform>();
            rect.anchoredPosition = position;
            rect.sizeDelta = new Vector2(180, 45);

            // Add text label
            CreateText(btnObj, "Label", labelText, 14, Vector2.zero, new Vector2(180, 45), TextAnchor.MiddleCenter);

            return btn;
        }

        private static InputField CreateInputField(GameObject parent, string name, string placeholderText, Vector2 position)
        {
            GameObject inputObj = new GameObject(name);
            inputObj.transform.SetParent(parent.transform, false);

            Image bgImage = inputObj.AddComponent<Image>();
            bgImage.color = new Color(0.08f, 0.05f, 0.12f);

            InputField input = inputObj.AddComponent<InputField>();
            RectTransform rect = inputObj.GetComponent<RectTransform>();
            rect.anchoredPosition = position;
            rect.sizeDelta = new Vector2(250, 40);

            // Placeholder Text
            Text placeholder = CreateText(inputObj, "Placeholder", placeholderText, 12, Vector2.zero, new Vector2(250, 40), TextAnchor.MiddleCenter);
            placeholder.color = new Color(0.6f, 0.6f, 0.6f);
            input.placeholder = placeholder;

            // Input Text
            Text textComponent = CreateText(inputObj, "Text", "", 14, Vector2.zero, new Vector2(250, 40), TextAnchor.MiddleCenter);
            textComponent.color = Color.white;
            input.textComponent = textComponent;

            return input;
        }
    }
}
#endif
