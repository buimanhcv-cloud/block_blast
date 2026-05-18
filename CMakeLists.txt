cmake_minimum_required(VERSION 3.10)
project(BlockPuzzleGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32 AND EXISTS "C:/SFML/lib/cmake/SFML")
    set(SFML_DIR "C:/SFML/lib/cmake/SFML")
endif()

find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

set(GAME_SOURCES
    main.cpp
    core/GameManager.cpp
    board/Board.cpp
    block/Block.cpp
    ui/Menu.cpp
    ui/HUD.cpp
    util/ResourceUtils.cpp
    tests/SelfTest.cpp
)

# Game executable
add_executable(${PROJECT_NAME} ${GAME_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE . core board block ui util)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system)

if (WIN32 AND EXISTS "C:/SFML/bin")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML/bin"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
