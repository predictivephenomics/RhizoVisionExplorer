# CMake script to fix rpaths in dylibs bundled by macdeployqt.
#
# macdeployqt rewrites the main binary's LC_RPATH to @executable_path/../Frameworks,
# but the bundled dylibs still carry the original build-tree rpath (e.g. the conda
# env's lib directory). This script replaces that absolute path with @loader_path so
# the dylibs resolve their own dependencies from within the bundle.
#
# Required variables (pass via -D on the command line):
#   BUNDLE_DIR        — path to the .app bundle
#   CONDA_LIB_PATH    — the absolute rpath to replace (e.g. /path/to/conda/envs/.../lib)

if(NOT BUNDLE_DIR)
    message(FATAL_ERROR "fix_bundle_rpaths.cmake: BUNDLE_DIR not set")
endif()
if(NOT CONDA_LIB_PATH)
    message(FATAL_ERROR "fix_bundle_rpaths.cmake: CONDA_LIB_PATH not set")
endif()

file(GLOB FRAMEWORK_DYLIBS "${BUNDLE_DIR}/Contents/Frameworks/*.dylib")

foreach(dylib ${FRAMEWORK_DYLIBS})
    execute_process(
        COMMAND otool -l "${dylib}"
        OUTPUT_VARIABLE otool_out
        ERROR_QUIET
    )
    if(otool_out MATCHES "${CONDA_LIB_PATH}")
        execute_process(
            COMMAND install_name_tool -rpath "${CONDA_LIB_PATH}" "@loader_path" "${dylib}"
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(WARNING "fix_bundle_rpaths: failed to patch ${dylib}")
        else()
            message(STATUS "Patched rpath in: ${dylib}")
        endif()
    endif()
endforeach()

# Plugins sit two directories deeper (Contents/PlugIns/<subdir>/), so they need
# to reach back up to Contents/Frameworks via @loader_path/../../Frameworks.
file(GLOB_RECURSE PLUGIN_DYLIBS "${BUNDLE_DIR}/Contents/PlugIns/*.dylib")

foreach(dylib ${PLUGIN_DYLIBS})
    execute_process(
        COMMAND otool -l "${dylib}"
        OUTPUT_VARIABLE otool_out
        ERROR_QUIET
    )
    if(otool_out MATCHES "${CONDA_LIB_PATH}")
        execute_process(
            COMMAND install_name_tool -rpath "${CONDA_LIB_PATH}" "@loader_path/../../Frameworks" "${dylib}"
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(WARNING "fix_bundle_rpaths: failed to patch plugin ${dylib}")
        else()
            message(STATUS "Patched rpath in plugin: ${dylib}")
        endif()
    endif()
endforeach()
