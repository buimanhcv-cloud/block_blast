# Mobile Port Plan

## Current status

The Windows build now runs with SFML and has fewer hard dependencies on local assets.

## Main blockers for phone support

1. The game loop, rendering, and input handling are still tied directly to SFML desktop APIs.
2. The board state and rendering state are mixed in the same classes.
3. Asset management is still file-path based instead of platform packaging based.

## Recommended path

1. Keep `core` focused on pure game rules.
2. Move board occupancy, scoring, block generation, and game-over rules into platform-neutral classes.
3. Keep SFML only as a desktop frontend.
4. Build a separate mobile frontend later using one of:
   - Android NDK + SDL
   - Flutter/Unity front-end over shared C++ logic
   - full Android rewrite if the game stays small

## Next refactor targets

1. Extract `GameSession` from `GameManager` with no SFML types.
2. Replace direct `sf::Vector2f` usage in logic with plain integers or simple structs.
3. Convert resources to a manifest-based loader.
4. Replace drag/drop code with action-based input commands: press, drag, release.
