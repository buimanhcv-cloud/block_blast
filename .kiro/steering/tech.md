# Tech Stack

## Language & Standard
- C++17 (`-std=c++17`)

## Libraries & Frameworks
- **SFML 2.5** — graphics, audio, window, system modules
  - Expected install path: `C:\SFML`
  - DLLs are copied to the output directory post-build
- **Node.js** — optional multiplayer server (`server.js`)

## Build Systems

### Primary: CMake + Visual Studio 2022 (recommended)
Generates an x64 MSVC solution targeting the `Release` config.

```bat
mkdir build_x64
cd build_x64
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Or use the convenience script from the repo root:
```bat
build_and_run.bat
```

### Alternative: MinGW / TDM-GCC-64 (direct g++ compile)
Requires TDM-GCC-64 at `C:\TDM-GCC-64` (or Dev-C++ bundled GCC).

```bat
build_windows.bat   # compile + copy DLLs
run_windows.bat     # launch the exe
```

### Quick launch shortcuts
```bat
Play_Game.bat       # run with console window
Play_Game.vbs       # run without console window
```

## Running Tests
Self-tests are compiled into the main executable and run via a CLI flag:

```bat
BlockPuzzleGame.exe --self-test
```

Tests live in `tests/SelfTest.cpp` and exercise `Board` and `Block` logic directly (no external test framework).

## SFML Dependency Notes
- SFML must be installed at `C:\SFML` for CMake auto-detection to work
- After a CMake build, SFML DLLs are auto-copied to the output directory via a `POST_BUILD` step
- For MinGW builds, `build_windows.bat` copies DLLs manually
- A portable distribution with pre-bundled DLLs lives in `BlockPuzzleGame_Portable/`

## Graphics Configuration
All window/rendering constants are centralised in `core/GraphicsConfig.h`:
- Resolution: 540×960
- Frame rate: 60 fps (VSync enabled by default)
- Anti-aliasing: level 4
