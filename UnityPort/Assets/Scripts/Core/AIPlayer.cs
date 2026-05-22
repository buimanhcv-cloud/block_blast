using UnityEngine;

namespace BlockBlast.Core
{
    public static class AIPlayer
    {
        public struct Move
        {
            public int trayIndex;
            public Vector2Int boardPos;
            public bool valid;
        }

        /// <summary>
        /// Finds the best possible placement for any of the available blocks in the tray.
        /// Returns Move.valid = false if no block can be placed anywhere.
        /// </summary>
        public static Move FindBestMove(Board board, Block[] tray)
        {
            Move bestMove = new Move { trayIndex = -1, boardPos = Vector2Int.zero, valid = false };
            int bestScore = int.MinValue;

            for (int t = 0; t < tray.Length; t++)
            {
                if (tray[t] == null) continue;
                Block block = tray[t];
                int[,] shape = block.Shape;

                int rows = shape.GetLength(0);
                int cols = shape.GetLength(1);

                // Try placing the block at every possible coordinate on the board
                // In Board.cs: CanPlaceBlock checks constraints at coordinate boardPos.
                // It checks: boardX = boardPos.x + c; boardY = boardPos.y - r;
                // Therefore, we scan board positions from (0, 0) up to (SIZE-1, SIZE-1).
                for (int bx = 0; bx < Board.SIZE; bx++)
                {
                    for (int by = 0; by < Board.SIZE; by++)
                    {
                        Vector2Int pos = new Vector2Int(bx, by);
                        if (board.CanPlaceBlock(shape, pos))
                        {
                            int score = ScorePlacement(board, shape, pos);
                            if (score > bestScore)
                            {
                                bestScore = score;
                                bestMove = new Move
                                {
                                    trayIndex = t,
                                    boardPos = pos,
                                    valid = true
                                };
                            }
                        }
                    }
                }
            }

            return bestMove;
        }

        /// <summary>
        /// Simulates placing the block shape at target position, scoring the resulting grid.
        /// Score calculation: (Lines Cleared * 100) - (Total Filled Cells).
        /// </summary>
        private static int ScorePlacement(Board board, int[,] shape, Vector2Int pos)
        {
            // Clone board grid
            int[,] tempGrid = new int[Board.SIZE, Board.SIZE];
            for (int x = 0; x < Board.SIZE; x++)
            {
                for (int y = 0; y < Board.SIZE; y++)
                {
                    tempGrid[x, y] = board.GetCell(x, y);
                }
            }

            // Simulate place
            int shapeRows = shape.GetLength(0);
            int shapeCols = shape.GetLength(1);
            for (int r = 0; r < shapeRows; r++)
            {
                for (int c = 0; c < shapeCols; c++)
                {
                    if (shape[r, c] != 0)
                    {
                        int boardX = pos.x + c;
                        int boardY = pos.y - r;
                        if (boardX >= 0 && boardX < Board.SIZE && boardY >= 0 && boardY < Board.SIZE)
                        {
                            tempGrid[boardX, boardY] = 1;
                        }
                    }
                }
            }

            // Check how many lines cleared
            int linesCleared = 0;
            bool[] fullRows = new bool[Board.SIZE];
            bool[] fullCols = new bool[Board.SIZE];

            for (int y = 0; y < Board.SIZE; y++)
            {
                bool rowFull = true;
                for (int x = 0; x < Board.SIZE; x++)
                {
                    if (tempGrid[x, y] == 0)
                    {
                        rowFull = false;
                        break;
                    }
                }
                fullRows[y] = rowFull;
                if (rowFull) linesCleared++;
            }

            for (int x = 0; x < Board.SIZE; x++)
            {
                bool colFull = true;
                for (int y = 0; y < Board.SIZE; y++)
                {
                    if (tempGrid[x, y] == 0)
                    {
                        colFull = false;
                        break;
                    }
                }
                fullCols[x] = colFull;
                if (colFull) linesCleared++;
            }

            // Apply simulation clears to grid to get accurate filled cells count
            for (int y = 0; y < Board.SIZE; y++)
            {
                if (fullRows[y])
                {
                    for (int x = 0; x < Board.SIZE; x++) tempGrid[x, y] = 0;
                }
            }
            for (int x = 0; x < Board.SIZE; x++)
            {
                if (fullCols[x])
                {
                    for (int y = 0; y < Board.SIZE; y++) tempGrid[x, y] = 0;
                }
            }

            // Count total filled cells left
            int filled = 0;
            for (int x = 0; x < Board.SIZE; x++)
            {
                for (int y = 0; y < Board.SIZE; y++)
                {
                    if (tempGrid[x, y] == 1) filled++;
                }
            }

            return linesCleared * 100 - filled;
        }
    }
}
