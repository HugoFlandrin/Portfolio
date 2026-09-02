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

# --- MarioLike game (Engine + Platformer) ---------------------------------
set(ENGINE_DIR "${CMAKE_SOURCE_DIR}/../Engine")
set(PLATFORMER_DIR "${CMAKE_SOURCE_DIR}/../Platformer")

file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS "${ENGINE_DIR}/*.cpp")

set(PLATFORMER_SOURCES
    "${PLATFORMER_DIR}/MarioLike.cpp"
    "${PLATFORMER_DIR}/FirstMarioScene.cpp"
    "${PLATFORMER_DIR}/MapBehavior.cpp"
    "${PLATFORMER_DIR}/PlayerBehavior.cpp"
    "${PLATFORMER_DIR}/EnemyBehavior.cpp"
    "${PLATFORMER_DIR}/SpikeBehavior.cpp"
    "${PLATFORMER_DIR}/FlagBehavior.cpp"
    "${PLATFORMER_DIR}/CoinBehavior.cpp"
    "${PLATFORMER_DIR}/ProjectileBehavior.cpp"
    "${PLATFORMER_DIR}/MenuScene.cpp"
    "${PLATFORMER_DIR}/SettingsScene.cpp"
    "${PLATFORMER_DIR}/GameOverScene.cpp"
    "${PLATFORMER_DIR}/TutorialHints.cpp"
    # WebGameOverScene.cpp intentionally excluded: it lives in Engine/ now
    # (it was already fully generic, no platformer-specific code), so it's
    # picked up by ENGINE_SOURCES above instead.
    # SecondMarioScene.cpp intentionally excluded: not registered in MarioLike.cpp,
    # kept in the repo for a possible future second level.
)

add_executable(platformer ${ENGINE_SOURCES} ${PLATFORMER_SOURCES})
target_include_directories(platformer PRIVATE "${ENGINE_DIR}" "${PLATFORMER_DIR}")
target_link_libraries(platformer PRIVATE SFML::Graphics SFML::Window SFML::System box2d)

set_target_properties(platformer PROPERTIES
    SUFFIX ".html"
    OUTPUT_NAME "index"
    # Both games produce index.html/.js/.wasm/.data - without a distinct
    # output directory per target, ninja errors out ("multiple rules
    # generate index.html") since platformer and shmup would collide.
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/platformer"
)
target_link_options(platformer PRIVATE
    "-sUSE_WEBGL2=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sGL_EXPLICIT_UNIFORM_LOCATION=1"
    "-sGL_EXPLICIT_UNIFORM_BINDING=1"
    "--preload-file=${PLATFORMER_DIR}/Assets@/Assets"
    "--shell-file=${CMAKE_SOURCE_DIR}/emscripten/shell.html"
    "-sEXPORTED_RUNTIME_METHODS=HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64"
    # VRSFML's Window::display() unconditionally yields to the browser via an
    # async primitive on Emscripten (requestAnimationFrame or emscripten_sleep,
    # depending on vsync) to let the browser drive frame timing - this requires
    # Asyncify support, there is no way around it while using this library.
    "-sASYNCIFY=1"
)

# --- Shmup game (Engine + Shmup) -------------------------------------------
set(SHMUP_DIR "${CMAKE_SOURCE_DIR}/../Shmup")

set(SHMUP_SOURCES
    "${SHMUP_DIR}/Shmup.cpp"
    "${SHMUP_DIR}/ShmupScene.cpp"
    "${SHMUP_DIR}/ShipBehavior.cpp"
    "${SHMUP_DIR}/BulletBehavior.cpp"
    "${SHMUP_DIR}/ShmupEnemyBehavior.cpp"
    "${SHMUP_DIR}/EnemySpawner.cpp"
    "${SHMUP_DIR}/ScrollingBackground.cpp"
    "${SHMUP_DIR}/ExplosionEffect.cpp"
    "${SHMUP_DIR}/ShmupControlHint.cpp"
)

add_executable(shmup ${ENGINE_SOURCES} ${SHMUP_SOURCES})
target_include_directories(shmup PRIVATE "${ENGINE_DIR}" "${SHMUP_DIR}")
target_link_libraries(shmup PRIVATE SFML::Graphics SFML::Window SFML::System box2d)

set_target_properties(shmup PROPERTIES
    SUFFIX ".html"
    OUTPUT_NAME "index"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/shmup"
)
target_link_options(shmup PRIVATE
    "-sUSE_WEBGL2=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sGL_EXPLICIT_UNIFORM_LOCATION=1"
    "-sGL_EXPLICIT_UNIFORM_BINDING=1"
    "--preload-file=${SHMUP_DIR}/Assets@/Assets"
    "--shell-file=${CMAKE_SOURCE_DIR}/emscripten/shell-shmup.html"
    "-sEXPORTED_RUNTIME_METHODS=HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64"
    "-sASYNCIFY=1"
)
