cmake_minimum_required(VERSION 3.28)
project(MarioLikeWeb CXX)

if(NOT CMAKE_TOOLCHAIN_FILE MATCHES "Emscripten")
    message(FATAL_ERROR "This project must be configured with emcmake (Emscripten toolchain).")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- VRSFML (Emscripten-ready SFML fork) ---------------------------------
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_IMGUI OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_OPENGL_ES ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/vittorioromeo/zancle.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SFML)

# --- Box2D (pinned to the same version used by the desktop/vcpkg build) --
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v3.1.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(box2d)

# --- Phase 0 validation target: hello-triangle ----------------------------
add_executable(hello_triangle hello_triangle.cpp)
target_link_libraries(hello_triangle PRIVATE SFML::Graphics SFML::Window SFML::System box2d)

set_target_properties(hello_triangle PROPERTIES SUFFIX ".html")
target_link_options(hello_triangle PRIVATE
    "-sUSE_WEBGL2=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sGL_EXPLICIT_UNIFORM_LOCATION=1"
    "-sGL_EXPLICIT_UNIFORM_BINDING=1"
)

# --- MarioLike game (Engine + Game) ---------------------------------------
set(ENGINE_DIR "${CMAKE_SOURCE_DIR}/../Engine")
set(GAME_DIR "${CMAKE_SOURCE_DIR}/../Game")

file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS "${ENGINE_DIR}/*.cpp")

set(GAME_SOURCES
    "${GAME_DIR}/MarioLike.cpp"
    "${GAME_DIR}/FirstMarioScene.cpp"
    "${GAME_DIR}/MapBehavior.cpp"
    "${GAME_DIR}/PlayerBehavior.cpp"
    "${GAME_DIR}/EnemyBehavior.cpp"
    "${GAME_DIR}/SpikeBehavior.cpp"
    "${GAME_DIR}/FlagBehavior.cpp"
    "${GAME_DIR}/CoinBehavior.cpp"
    "${GAME_DIR}/ProjectileBehavior.cpp"
    "${GAME_DIR}/MenuScene.cpp"
    "${GAME_DIR}/SettingsScene.cpp"
    "${GAME_DIR}/GameOverScene.cpp"
    "${GAME_DIR}/WebGameOverScene.cpp"
    "${GAME_DIR}/TutorialHints.cpp"
    # SecondMarioScene.cpp intentionally excluded: not registered in MarioLike.cpp,
    # kept in the repo for a possible future second level.
)

add_executable(game ${ENGINE_SOURCES} ${GAME_SOURCES})
target_include_directories(game PRIVATE "${ENGINE_DIR}" "${GAME_DIR}")
target_link_libraries(game PRIVATE SFML::Graphics SFML::Window SFML::System box2d)

set_target_properties(game PROPERTIES SUFFIX ".html" OUTPUT_NAME "index")
target_link_options(game PRIVATE
    "-sUSE_WEBGL2=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sGL_EXPLICIT_UNIFORM_LOCATION=1"
    "-sGL_EXPLICIT_UNIFORM_BINDING=1"
    "--preload-file=${GAME_DIR}/Assets@/Assets"
    "--shell-file=${CMAKE_SOURCE_DIR}/emscripten/shell.html"
    "-sEXPORTED_RUNTIME_METHODS=HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64"
    # VRSFML's Window::display() unconditionally yields to the browser via an
    # async primitive on Emscripten (requestAnimationFrame or emscripten_sleep,
    # depending on vsync) to let the browser drive frame timing - this requires
    # Asyncify support, there is no way around it while using this library.
    "-sASYNCIFY=1"
)
