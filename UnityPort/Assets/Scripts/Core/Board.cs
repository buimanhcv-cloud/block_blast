using UnityEngine;

namespace BlockBlast.Core
{
    public class Board : MonoBehaviour
    {
        public const int SIZE = 8;

        [Header("Grid Layout Settings")]
        public float cellSize = 1.0f;
        public Vector2 gridOffset = new Vector2(-3.5f, -3.5f); // Centers the 8x8 grid around (0,0) in World Space

        private int[,] grid = new int[SIZE, SIZE];

        public int[,] Grid => grid;

        private void Awake()
        {
            ResetBoard();
        }

        public void ResetBoard()
        {
            for (int x = 0; x < SIZE; x++)
            {
                for (int y = 0; y < SIZE; y++)
                {
                    grid[x, y] = 0;
                }
            }
        }

        /// <summary>
        /// Checks if a block shape can be placed at the specified board grid coordinates.
        /// </summary>
        public bool CanPlaceBlock(int[,] shape, Vector2Int boardPos)
        {
            int shapeRows = shape.GetLength(0);
            int shapeCols = shape.GetLength(1);

            for (int r = 0; r < shapeRows; r++)
            {
                for (int c = 0; c < shapeCols; c++)
                {
                    if (shape[r, c] != 0)
                    {
                        int boardX = boardPos.x + c;
                        int boardY = boardPos.y - r; // In C++, shape rows go down (y increases). In Unity, 2D coordinates y decreases downwards.

                        if (boardX < 0 || boardX >= SIZE || boardY < 0 || boardY >= SIZE)
                        {
                            return false;
                        }

                        if (grid[boardX, boardY] == 1)
                        {
                            return false;
                        }
                    }
                }
            }
            return true;
        }

        /// <summary>
        /// Places a block shape onto the board grid at the specified coordinates.
        /// </summary>
        public void PlaceBlock(int[,] shape, Vector2Int boardPos)
        {
            int shapeRows = shape.GetLength(0);
            int shapeCols = shape.GetLength(1);

            for (int r = 0; r < shapeRows; r++)
            {
                for (int c = 0; c < shapeCols; c++)
                {
                    if (shape[r, c] != 0)
                    {
                        int boardX = boardPos.x + c;
                        int boardY = boardPos.y - r;
                        if (boardX >= 0 && boardX < SIZE && boardY >= 0 && boardY < SIZE)
                        {
                            grid[boardX, boardY] = 1;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Clears all full rows and columns. Returns the number of lines cleared.
        /// </summary>
        public int ClearFullLines()
        {
            int cleared = 0;
            bool[] fullRows = new bool[SIZE];
            bool[] fullCols = new bool[SIZE];

            // Check full rows
            for (int y = 0; y < SIZE; y++)
            {
                bool rowFull = true;
                for (int x = 0; x < SIZE; x++)
                {
                    if (grid[x, y] == 0)
                    {
                        rowFull = false;
                        break;
                    }
                }
                fullRows[y] = rowFull;
                if (rowFull) cleared++;
            }

            // Check full columns
            for (int x = 0; x < SIZE; x++)
            {
                bool colFull = true;
                for (int y = 0; y < SIZE; y++)
                {
                    if (grid[x, y] == 0)
                    {
                        colFull = false;
                        break;
                    }
                }
                fullCols[x] = colFull;
                if (colFull) cleared++;
            }

            // Clear row data
            for (int y = 0; y < SIZE; y++)
            {
                if (fullRows[y])
                {
                    for (int x = 0; x < SIZE; x++)
                    {
                        grid[x, y] = 0;
                    }
                }
            }

            // Clear col data
            for (int x = 0; x < SIZE; x++)
            {
                if (fullCols[x])
                {
                    for (int y = 0; y < SIZE; y++)
                    {
                        grid[x, y] = 0;
                    }
                }
            }

            return cleared;
        }

        /// <summary>
        /// Checks if a block can be placed ANYWHERE on the board.
        /// </summary>
        public bool CanPlaceAnyBlock(int[,] shape)
        {
            int shapeRows = shape.GetLength(0);
            int shapeCols = shape.GetLength(1);

            for (int boardX = 0; boardX < SIZE; boardX++)
            {
                for (int boardY = 0; boardY < SIZE; boardY++)
                {
                    bool fits = true;

                    for (int r = 0; r < shapeRows && fits; r++)
                    {
                        for (int c = 0; c < shapeCols; c++)
                        {
                            if (shape[r, c] == 0) continue;

                            int x = boardX + c;
                            int y = boardY - r;

                            if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || grid[x, y] == 1)
                            {
                                fits = false;
                                break;
                            }
                        }
                    }

                    if (fits) return true;
                }
            }

            return false;
        }

        /// <summary>
        /// Converts a world space coordinate (e.g., block position) to board cell index.
        /// </summary>
        public Vector2Int GetBoardPosition(Vector3 worldPos)
        {
            float relativeX = worldPos.x - gridOffset.x;
            float relativeY = worldPos.y - gridOffset.y;

            int x = Mathf.RoundToInt(relativeX / cellSize);
            int y = Mathf.RoundToInt(relativeY / cellSize);

            return new Vector2Int(x, y);
        }

        /// <summary>
        /// Gets the center world position of a specific cell coordinate on the board.
        /// </summary>
        public Vector3 GetCellWorldPosition(int x, int y)
        {
            return new Vector3(
                gridOffset.x + x * cellSize,
                gridOffset.y + y * cellSize,
                0f
            );
        }

        public int GetCell(int x, int y)
        {
            if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return -1;
            return grid[x, y];
        }
    }
}
