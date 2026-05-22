# Project Structure

## Source Modules

```
app/
├── main.cpp                  # Entry point: window setup, game loop, --self-test flag
│
├── core/
│   ├── GameManager.h/.cpp    # Central controller: state machine, input, tray, scoring
│   └── GraphicsConfig.h      # Window/render constants (resolution, FPS, AA level)
│
├── board/
│   └── Board.h/.cpp          # 8×8 grid logic: placement, line clearing, ghost cells
│
├── block/
│   └── Block.h/.cpp          # Block shapes, rendering, drag hit-testing, preview
│
├── ui/
│   ├── Menu.h/.cpp           # Main menu and game-over screen rendering
│   ├── HUD.h/.cpp            # Score/high-score overlay
│   ├── NetworkClient.h/.cpp  # (partial) WebSocket client for online mode
│   ├── GameNetworkManager.h/.cpp  # (partial) Network game state sync
│   ├── OnlineGameMode.h/.cpp      # (partial) Online PvP game mode
│   └── OnlineGameSelectScreen.h/.cpp  # (partial) Online lobby/select screen
│
├── util/
│   └── ResourceUtils.h/.cpp  # Font and sound loading helpers (graceful fallback)
│
├── tests/
│   └── SelfTest.h/.cpp       # In-process unit tests for Board and Block logic
│
├── launcher/
│   └── *.h/.cpp              # Standalone launcher GUI (currently has linker issues)
│
└── assets/                   # Optional runtime assets (fonts, sounds) — not committed
```

## Build Output Directories
- `build_x64/` — CMake + VS2022 x64 build (primary)
- `build_vs2022/` — alternate VS2022 CMake build
- `build/` — legacy/experimental build directory

## Portable Distribution
`BlockPuzzleGame_Portable/` contains a self-contained release build with all required DLLs bundled.

## Key Architectural Patterns

- **Header guards** — all headers use `#ifndef / #define / #endif` (no `#pragma once`)
- **Include paths** — modules include each other via relative paths from root (e.g. `"board/Board.h"`, `"block/Block.h"`); CMake sets `-I.` plus per-module `-I` flags
- **Ownership** — `GameManager` owns `Board`, `Menu`, `HUD`, and tray `Block`s via `std::unique_ptr`
- **Rendering** — each class exposes a `render(sf::RenderWindow&)` method; `GameManager::render()` orchestrates draw order
- **Constants** — magic numbers for layout (cell size, offsets, window size) live in the owning class as `static const` / `static constexpr` members or in `GraphicsConfig.h`
- **Assets** — loaded with graceful fallback in `ResourceUtils`; missing font or audio files do not crash the game
- **Tests** — `SelfTest.cpp` uses a simple `check(bool, const char*)` helper; no external framework
