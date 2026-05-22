using UnityEngine;
using UnityEngine.UI;

namespace BlockBlast.UI
{
    using BlockBlast.Network;

    public class OnlineGameSelectController : MonoBehaviour
    {
        [Header("UI Inputs")]
        public InputField hostInput;
        public InputField portInput;
        public InputField roomIdInput;
        public Dropdown modeDropdown;
        public Dropdown timeDropdown;

        [Header("Status & Actions")]
        public Text statusText;
        public Button connectButton;
        public Button backButton;

        [Header("References")]
        public GameNetworkManager netManager;
        public MenuController menuController;

        private void Start()
        {
            // Set defaults
            // Set defaults
            if (hostInput != null) hostInput.text = "weight-trustless.gl.at.ply.gg";
            if (portInput != null) portInput.text = "33412";
            if (roomIdInput != null) roomIdInput.text = "1234";

            if (connectButton != null) connectButton.onClick.AddListener(OnConnectClick);
            if (backButton != null) backButton.onClick.AddListener(OnBackClick);
        }

        private async void OnConnectClick()
        {
            if (netManager == null) return;

            string host = hostInput != null ? hostInput.text : "weight-trustless.gl.at.ply.gg";
            int port = 33412;
            if (portInput != null) int.TryParse(portInput.text, out port);

            string roomId = roomIdInput != null ? roomIdInput.text : "1234";

            OnlineMode mode = OnlineMode.REALTIME;
            if (modeDropdown != null) mode = (OnlineMode)modeDropdown.value;

            TimePreset preset = TimePreset.NORMAL;
            if (timeDropdown != null)
            {
                switch (timeDropdown.value)
                {
                    case 0: preset = TimePreset.FAST; break;
                    case 1: preset = TimePreset.NORMAL; break;
                    case 2: preset = TimePreset.LONG; break;
                }
            }

            SetStatus("Connecting...");
            connectButton.interactable = false;

            bool ok = await netManager.ConnectAsync(host, port);
            if (!ok)
            {
                SetStatus("Connection failed. Check server.js.");
                connectButton.interactable = true;
                return;
            }

            SetStatus($"Connected. Joining room {roomId}...");
            netManager.JoinRoom(roomId, mode, preset);
        }

        private void OnBackClick()
        {
            if (netManager != null) netManager.Disconnect();
            if (menuController != null) menuController.ShowMainMenu();
        }

        private void SetStatus(string text)
        {
            if (statusText != null)
            {
                statusText.text = text;
            }
        }
    }
}
