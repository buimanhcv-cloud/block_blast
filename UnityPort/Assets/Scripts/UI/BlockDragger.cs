using UnityEngine;

namespace BlockBlast.UI
{
    using BlockBlast.Core;

    [RequireComponent(typeof(Block))]
    [RequireComponent(typeof(Collider2D))]
    public class BlockDragger : MonoBehaviour
    {
        private Block block;
        private Vector3 originalPosition;
        private Vector3 dragOffset;
        private bool isDragging = false;
        private Camera mainCamera;
        private GameManager gameManager;

        public bool IsDragging => isDragging;

        private void Awake()
        {
            block = GetComponent<Block>();
            mainCamera = Camera.main;
            gameManager = FindObjectOfType<GameManager>();
        }

        public void SetOriginalPosition(Vector3 pos)
        {
            originalPosition = pos;
            transform.position = pos;
        }

        private void OnMouseDown()
        {
            if (gameManager != null && !gameManager.CanInteract(block)) return;

            isDragging = true;
            block.SetPreviewMode(false); // scale back up to full size during drag
            dragOffset = transform.position - GetMouseWorldPos();

            if (gameManager != null)
            {
                gameManager.OnBlockDragStart(block);
            }
        }

        private void OnMouseDrag()
        {
            if (!isDragging) return;

            transform.position = GetMouseWorldPos() + dragOffset;

            if (gameManager != null)
            {
                gameManager.OnBlockDragUpdate(block);
            }
        }

        private void OnMouseUp()
        {
            if (!isDragging) return;
            isDragging = false;

            if (gameManager != null)
            {
                bool placed = gameManager.OnBlockDragEnd(block);
                if (!placed)
                {
                    // Return to tray and preview mode
                    ReturnToTray();
                }
            }
            else
            {
                ReturnToTray();
            }
        }

        public void ReturnToTray()
        {
            transform.position = originalPosition;
            block.SetPreviewMode(true);
        }

        private Vector3 GetMouseWorldPos()
        {
            Vector3 mousePoint = Input.mousePosition;
            mousePoint.z = Mathf.Abs(mainCamera.transform.position.z); // distance to camera
            return mainCamera.ScreenToWorldPoint(mousePoint);
        }
    }
}
