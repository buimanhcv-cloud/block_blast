using System.Collections.Generic;
using UnityEngine;

namespace BlockBlast.Core
{
    public class Block : MonoBehaviour
    {
        public static readonly List<int[,]> ALL_SHAPES = new List<int[,]>
        {
            // Single
            new int[,] { { 1 } },
            // Dominoes
            new int[,] { { 1, 1 } },
            new int[,] { { 1 }, { 1 } },
            // Triominoes
            new int[,] { { 1, 1, 1 } },
            new int[,] { { 1 }, { 1 }, { 1 } },
            new int[,] { { 1, 1 }, { 1, 0 } },
            new int[,] { { 1, 1 }, { 0, 1 } },
            new int[,] { { 1, 0 }, { 1, 1 } },
            new int[,] { { 0, 1 }, { 1, 1 } },
            // Tetrominoes
            new int[,] { { 1, 1, 1, 1 } },
            new int[,] { { 1 }, { 1 }, { 1 }, { 1 } },
            new int[,] { { 1, 1 }, { 1, 1 } },
            new int[,] { { 1, 1, 1 }, { 1, 0, 0 } },
            new int[,] { { 1, 1, 1 }, { 0, 0, 1 } },
            new int[,] { { 1, 0, 0 }, { 1, 1, 1 } },
            new int[,] { { 0, 0, 1 }, { 1, 1, 1 } },
            new int[,] { { 1, 1, 1 }, { 0, 1, 0 } },
            // Pentominoes
            new int[,] { { 1, 1, 1 }, { 1, 0, 0 }, { 1, 0, 0 } },
            new int[,] { { 1, 1, 1 }, { 0, 0, 1 }, { 0, 0, 1 } }
        };

        public static readonly List<Color> SHAPE_COLORS = new List<Color>
        {
            new Color(0.52f, 0.76f, 1f),    // single - light blue
            new Color(0.96f, 0.68f, 0.9f),   // domino h - pink
            new Color(0.96f, 0.68f, 0.9f),   // domino v - pink
            new Color(0.36f, 0.83f, 1f),    // tri h - cyan
            new Color(0.36f, 0.83f, 1f),    // tri v - cyan
            new Color(0.92f, 0.4f, 0.55f),   // tri L1 - rose
            new Color(0.92f, 0.4f, 0.55f),   // tri L2 - rose
            new Color(0.92f, 0.4f, 0.55f),   // tri L3 - rose
            new Color(0.92f, 0.4f, 0.55f),   // tri L4 - rose
            new Color(1f, 0.81f, 0.36f),     // I4 h - yellow
            new Color(1f, 0.81f, 0.36f),     // I4 v - yellow
            new Color(0.71f, 0.51f, 1f),     // O - purple
            new Color(1f, 0.55f, 0.35f),     // L1 - orange
            new Color(1f, 0.55f, 0.35f),     // L2 - orange
            new Color(1f, 0.55f, 0.35f),     // L3 - orange
            new Color(1f, 0.55f, 0.35f),     // L4 - orange
            new Color(0.39f, 0.9f, 0.71f),    // T - teal
            new Color(1f, 0.71f, 0.31f),     // P1 - amber
            new Color(1f, 0.71f, 0.31f)      // P2 - amber
        };

        [Header("Prefabs & Display")]
        public GameObject cellPrefab; // A simple quad/sprite prefab representing a cell
        public float cellSize = 1.0f;
        public float trayPreviewScale = 0.55f;

        private int[,] shape;
        private Color blockColor;
        private List<GameObject> activeCells = new List<GameObject>();
        private int shapeIndex = -1;

        public int[,] Shape => shape;
        public Color BlockColor => blockColor;
        public int ShapeIndex => shapeIndex;

        /// <summary>
        /// Initializes a random block shape.
        /// </summary>
        public void InitializeRandom()
        {
            int idx = Random.Range(0, ALL_SHAPES.Count);
            Initialize(idx);
        }

        /// <summary>
        /// Initializes a block with a specific shape index.
        /// </summary>
        public void Initialize(int index)
        {
            shapeIndex = index;
            shape = ALL_SHAPES[index];
            blockColor = SHAPE_COLORS[index];
            BuildCellsVisuals();
        }

        /// <summary>
        /// Custom initialization directly from a custom shape matrix.
        /// </summary>
        public void InitializeCustom(int[,] customShape, Color customColor)
        {
            shapeIndex = -1;
            shape = customShape;
            blockColor = customColor;
            BuildCellsVisuals();
        }

        private void BuildCellsVisuals()
        {
            // Clear existing cell visual instances
            foreach (var cell in activeCells)
            {
                if (cell != null) Destroy(cell);
            }
            activeCells.Clear();

            if (shape == null || cellPrefab == null) return;

            int rows = shape.GetLength(0);
            int cols = shape.GetLength(1);

            // Calculate local center to position them relative to block origin
            // Block origin will be top-left or centered. 
            // In C++, the drag origin is at the top-left offset, but let's center it.
            float halfWidth = (cols * cellSize) / 2f;
            float halfHeight = (rows * cellSize) / 2f;

            for (int r = 0; r < rows; r++)
            {
                for (int c = 0; c < cols; c++)
                {
                    if (shape[r, c] != 0)
                    {
                        GameObject cellInstance = Instantiate(cellPrefab, transform);
                        // Offset each cell relative to the pivot.
                        // Row index r goes down, so we subtract from Y.
                        cellInstance.transform.localPosition = new Vector3(
                            (c * cellSize + cellSize / 2f) - halfWidth,
                            -(r * cellSize + cellSize / 2f) + halfHeight,
                            0f
                        );

                        // Configure cell color
                        var renderer = cellInstance.GetComponentInChildren<SpriteRenderer>();
                        if (renderer != null)
                        {
                            renderer.color = blockColor;
                        }
                        activeCells.Add(cellInstance);
                    }
                }
            }
        }

        /// <summary>
        /// Scales the block for tray preview.
        /// </summary>
        public void SetPreviewMode(bool isPreview)
        {
            float scale = isPreview ? trayPreviewScale : 1.0f;
            transform.localScale = new Vector3(scale, scale, 1.0f);
        }

        /// <summary>
        /// Get boundary sizes of the block.
        /// </summary>
        public Vector2 GetSize()
        {
            if (shape == null) return Vector2.zero;
            int rows = shape.GetLength(0);
            int cols = shape.GetLength(1);
            return new Vector2(cols * cellSize, rows * cellSize);
        }
    }
}
