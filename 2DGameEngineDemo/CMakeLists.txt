cmake_minimum_required(VERSION 3.28)
project(2DGameEngineDemo CXX)

# Native (Windows/desktop) build - open this folder directly in Visual
# Studio 2022 (File > Open > Folder) for full IDE support (F5, breakpoints,
# IntelliSense): VS auto-detects this CMakeLists.txt and configures itself.
#
# This exists because the Engine/Game code was fully ported to VRSFML's API
# (sf::Vec2f, sf::Utf8String, designated initializers, non-owning
# Sprite/Texture, ...) for the web build - vanilla SFML (what the old
# vcpkg-based .vcxproj files link against) does not have that API at all,
# so those old desktop projects no longer compile. Rather than reverting the
# whole engine back to vanilla SFML, this builds VRSFML itself (the same
# fork/version the web build uses) natively for Windows instead of through
# Emscripten - same source, same API, different target platform.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VRSFML requires C++23 (see its Config.hpp) and sets CXX_STANDARD 23 on its
# own targets. This CMake version has no clang-cl mapping for C++23 yet, so
# it falls back to the raw "-clang:-std=c++23" passthrough flag, which makes
# the underlying clang frontend parse C++23 but does NOT set _MSVC_LANG
# (clang-cl only updates that from a driver-level /std: flag) - and both
# SFML/Config.hpp and the MSVC STL gate their C++23 checks on _MSVC_LANG, not
# on the actual parsing mode (confirmed: adding /std:c++23 via
# add_compile_options was silently ignored as "unused" because the
# passthrough flag already "claimed" the standard as far as clang-cl's
# argument parser is concerned). Overriding the compile-option variables
# CMake itself uses to translate CXX_STANDARD 23 makes it emit /std:c++23
# directly instead of the passthrough, which does set _MSVC_LANG correctly.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # Plain set(), not CACHE: CMake's Clang-CXX module already set these as
    # normal variables during project() above, and a normal variable shadows
    # a same-named CACHE entry in this scope - only a plain re-set actually
    # takes effect and propagates into FetchContent's subdirectories.
    #
    # /std:c++23 (confirmed via a standalone clang-cl invocation) is silently
    # rejected as an unrecognized /std: value by this clang-cl build (LLVM
    # 19.1.5, bundled with VS 17.0) - it warns "argument unused" and falls
    # back to its ancient default standard instead of erroring, which is why
    # the C++23 checks kept failing even after removing the raw "-clang:
    # -std=c++23" passthrough. /std:c++latest IS recognized and maps to the
    # newest standard this clang-cl knows (C++23 here), and correctly sets
    # _MSVC_LANG.
    set(CMAKE_CXX23_STANDARD_COMPILE_OPTION "/std:c++latest")
    set(CMAKE_CXX23_EXTENSION_COMPILE_OPTION "/std:c++latest")

    # This clang-cl build (LLVM 19.1.5) has its own bug on top of the above:
    # under /std:c++latest it correctly bumps __cplusplus to 202400L but
    # leaves _MSVC_LANG stuck at 202004L (confirmed via -dM -E) - and both
    # SFML/Config.hpp and the MSVC STL's C++23 feature gates (e.g.
    # std::chrono::file_clock::to_sys) check _MSVC_LANG first whenever it's
    # defined, ignoring the correct __cplusplus value entirely. Forcing it
    # via a command-line define is the only way to make clang-cl report a
    # consistent, correct value here.
    #
    # CXX-only: _MSVC_LANG is meaningless for C translation units, but SDL3
    # (a C dependency pulled in by VRSFML) has Vulkan headers that key off
    # "defined(_MSVC_LANG)" to decide whether nullptr is available - applying
    # this define to C files too made them see _MSVC_LANG defined and emit
    # nullptr in plain C, which doesn't exist there (confirmed: broke
    # SDL_vulkan_utils.c with "use of undeclared identifier 'nullptr'").
    add_compile_definitions($<$<COMPILE_LANGUAGE:CXX>:_MSVC_LANG=202302L>)
endif()

include(FetchContent)

# --- VRSFML (same fork/version as the web build) --------------------------
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_IMGUI OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
# Defaults to ON in Debug builds; its stack-trace capture relies on
# <cxxabi.h> (Itanium C++ ABI - libstdc++/libc++ only), which doesn't exist
# under clang-cl's MSVC-compatible standard library.
set(SFML_ENABLE_STACK_TRACES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/vittorioromeo/zancle.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    PATCH_COMMAND ${CMAKE_COMMAND}
        "-DSFML_PATCH_TARGET=<SOURCE_DIR>/src/SFML/System/Unity/Path.cpp"
        -P "${CMAKE_SOURCE_DIR}/cmake/patch_sfml_path.cmake"
)
FetchContent_MakeAvailable(SFML)

# --- Box2D (pinned to the same version as the web build) -------------------
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

# --- Shared engine sources ---------------------------------------------
set(ENGINE_DIR "${CMAKE_SOURCE_DIR}/Engine")
set(PLATFORMER_DIR "${CMAKE_SOURCE_DIR}/Platformer")
set(SHMUP_DIR "${CMAKE_SOURCE_DIR}/Shmup")
set(GAMEHUB_DIR "${CMAKE_SOURCE_DIR}/GameHub")

file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS "${ENGINE_DIR}/*.cpp")

# WebBridge.cpp is entirely #ifdef __EMSCRIPTEN__ internally (nothing to
# compile for a native build) and isn't needed here at all. WebGameOverScene
# calls into WebBridge unconditionally and isn't registered/used by any
# native scene flow (GameOverScene is what Platformer/Shmup/GameHub actually
# use) - it's web-only, so exclude it too rather than link a stub WebBridge.
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "WebBridge\\.cpp$")
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "WebGameOverScene\\.cpp$")

set(PLATFORMER_SOURCES
    "${PLATFORMER_DIR}/MarioLike.cpp"
    "${PLATFORMER_DIR}/FirstMarioScene.cpp"
    "${PLATFORMER_DIR}/MapBehavior.cpp"
    "${PLATFORMER_DIR}/PlayerBehavior.cpp"
    "${PLATFORMER_DIR}/EnemyBehavior.cpp"
    "${PLATFORMER_DIR}/SpikeBehavior.cpp"
    "${PLATFORMER_DIR}/FlagBehavior.cpp"
    "${PLATFORMER_DIR}/CoinBehavior.cpp"
    "${PLATFORMER_DIR}/MenuScene.cpp"
    "${PLATFORMER_DIR}/SettingsScene.cpp"
    "${PLATFORMER_DIR}/GameOverScene.cpp"
)

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

# Platformer sources minus MarioLike.cpp (its own main()) - reused by gamehub.
set(PLATFORMER_LIB_SOURCES ${PLATFORMER_SOURCES})
list(REMOVE_ITEM PLATFORMER_LIB_SOURCES "${PLATFORMER_DIR}/MarioLike.cpp")

# Shmup sources minus Shmup.cpp (its own main()) - reused by gamehub.
set(SHMUP_LIB_SOURCES ${SHMUP_SOURCES})
list(REMOVE_ITEM SHMUP_LIB_SOURCES "${SHMUP_DIR}/Shmup.cpp")

set(GAMEHUB_SOURCES
    "${GAMEHUB_DIR}/GameHub.cpp"
    "${GAMEHUB_DIR}/GameSelectScene.cpp"
)

# Copies a game's Assets/ folder flat next to its .exe (matches
# ResourceManager's desktop path resolution: <exe_dir>\<relative_path>,
# no "Assets" subfolder).
function(copy_assets_for target)
    foreach(assetsDir ${ARGN})
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${assetsDir}" "$<TARGET_FILE_DIR:${target}>"
        )
    endforeach()
endfunction()

# --- Platformer -------------------------------------------------------
add_executable(platformer ${ENGINE_SOURCES} ${PLATFORMER_SOURCES})
target_include_directories(platformer PRIVATE "${ENGINE_DIR}" "${PLATFORMER_DIR}")
target_link_libraries(platformer PRIVATE SFML::Graphics SFML::Window SFML::System box2d)
copy_assets_for(platformer "${PLATFORMER_DIR}/Assets")

# --- Shmup --------------------------------------------------------------
# Pulls in the Platformer's GameOverScene too (see Shmup.cpp) - the native
# desktop build has no web page to notify like WebGameOverScene does, so it
# reuses the same fully generic scene the Platformer/GameHub already use.
add_executable(shmup ${ENGINE_SOURCES} ${SHMUP_SOURCES} "${PLATFORMER_DIR}/GameOverScene.cpp")
target_include_directories(shmup PRIVATE "${ENGINE_DIR}" "${SHMUP_DIR}" "${PLATFORMER_DIR}")
target_link_libraries(shmup PRIVATE SFML::Graphics SFML::Window SFML::System box2d)
copy_assets_for(shmup "${SHMUP_DIR}/Assets")

# --- GameHub (both games, starts on a game-select menu) -----------------
add_executable(gamehub ${ENGINE_SOURCES} ${PLATFORMER_LIB_SOURCES} ${SHMUP_LIB_SOURCES} ${GAMEHUB_SOURCES})
target_include_directories(gamehub PRIVATE "${ENGINE_DIR}" "${PLATFORMER_DIR}" "${SHMUP_DIR}" "${GAMEHUB_DIR}")
target_link_libraries(gamehub PRIVATE SFML::Graphics SFML::Window SFML::System box2d)
copy_assets_for(gamehub "${PLATFORMER_DIR}/Assets" "${SHMUP_DIR}/Assets")

# GameHub is the one with a menu to pick a game - make it the default
# selected/startup target when opening this folder in Visual Studio.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT gamehub)
