# MSVC's STL (confirmed across every installed toolset, 14.36 through
# 14.44) never implemented std::chrono::file_clock::to_sys/from_sys, which
# VRSFML's Path.cpp uses. Swap it for the portable pre-standardization idiom
# (diffing against "now" in both clocks) that works on any STL.
# Expects -DSFML_PATCH_TARGET=<path to Path.cpp> on the command line.

file(READ "${SFML_PATCH_TARGET}" contents)
string(FIND "${contents}" "std::chrono::file_clock::to_sys(ftime)" match_pos)
if(match_pos EQUAL -1)
    message(STATUS "SFML Path.cpp already patched (or pattern not found) - skipping")
else()
    string(REPLACE
        "std::chrono::file_clock::to_sys(ftime)"
        "std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::chrono::file_clock::now() + std::chrono::system_clock::now())"
        contents "${contents}")
    file(WRITE "${SFML_PATCH_TARGET}" "${contents}")
    message(STATUS "Patched SFML Path.cpp: file_clock::to_sys -> portable fallback (MSVC STL lacks it)")
endif()
