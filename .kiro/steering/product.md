# Product: Block Puzzle Game

A desktop block-puzzle game written in C++ with SFML. Players drag Tetris-style blocks from a 3-slot tray onto an 8×8 grid. Completed rows or columns are cleared for points. The game ends when no block in the tray can be placed.

## Core Gameplay
- 19 block shapes: single, domino, triomino, tetromino, pentomino
- Tray holds 3 blocks at a time; all 3 must be placed before the tray refills
- Combo multiplier (×2, ×3…) for clearing multiple lines in one move
- High score tracked per session; "NEW BEST!" shown on game over
- Ghost-cell preview (semi-transparent blue overlay) while dragging a block

## Game States
- `MENU` → `PLAYING` → `GAME_OVER` (managed by `GameManager`)

## Optional Online Multiplayer
A Node.js server (`server.js`) and client-side network classes (`ui/NetworkClient`, `ui/OnlineGameMode`, `ui/GameNetworkManager`) exist for a PvP mode. This feature is partially implemented.

## Assets (optional, placed in `assets/`)
- `arial.ttf` — fallback font
- `place.wav`, `clear.wav`, `combo.wav`, `gameover.wav` — sound effects
