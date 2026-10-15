# macOS packaging: runs macdeployqt after build, fixes bundled-dylib rpaths,
# and configures CPack to produce a drag-and-drop .dmg installer.

# Locate macdeployqt relative to where Qt6 was found.
# Qt6_DIR is typically <prefix>/lib/cmake/Qt6; macdeployqt lives at:
#   conda layout   → <prefix>/lib/qt6/bin/macdeployqt
#   standard layout → <prefix>/bin/macdeployqt
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
    HINTS
        "${Qt6_DIR}/../../../lib/qt6/bin"
        "${Qt6_DIR}/../../../bin"
    NO_DEFAULT_PATH
)

if(NOT MACDEPLOYQT_EXECUTABLE)
    message(WARNING "macdeployqt not found — the .app bundle will not be self-contained. "
                    "Hint: set Qt6_DIR to the cmake directory inside your Qt installation.")
    return()
endif()

message(STATUS "Found macdeployqt: ${MACDEPLOYQT_EXECUTABLE}")

# Determine the conda/prefix lib path from CMAKE_PREFIX_PATH (first entry wins).
# This is the rpath that macdeployqt leaves behind in bundled dylibs; we replace
# it with @loader_path so the bundle works on machines without the conda env.
list(GET CMAKE_PREFIX_PATH 0 _prefix)
set(_conda_lib "${_prefix}/lib")

# After the build, deploy Qt into the .app, fix the remaining rpaths, then
# re-sign the whole bundle with an ad-hoc identity.  The signing step is
# required because install_name_tool (used by both macdeployqt and the rpath
# fix) invalidates any existing code signatures, and macOS will SIGKILL the
# process at load time if the signature doesn't match the binary pages.
add_custom_command(TARGET RhizoVisionExplorer POST_BUILD
    COMMAND ${MACDEPLOYQT_EXECUTABLE}
        "$<TARGET_BUNDLE_DIR:RhizoVisionExplorer>"
        -verbose=1
        -no-strip
        "-libpath=${_conda_lib}"
    COMMAND ${CMAKE_COMMAND}
        "-DBUNDLE_DIR=$<TARGET_BUNDLE_DIR:RhizoVisionExplorer>"
        "-DCONDA_LIB_PATH=${_conda_lib}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/fix_bundle_rpaths.cmake"
    COMMAND codesign
        --force
        --deep
        --sign -
        "$<TARGET_BUNDLE_DIR:RhizoVisionExplorer>"
    COMMENT "Deploying Qt frameworks, fixing bundle rpaths, and ad-hoc signing"
    VERBATIM
)

# CPack: DragNDrop generator produces a .dmg with a symlink to /Applications.
set(CPACK_GENERATOR "DragNDrop")
set(CPACK_PACKAGE_NAME "RhizoVisionExplorer")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Oak Ridge National Laboratory")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RhizoVision Explorer — root image analysis tool")
set(CPACK_DMG_VOLUME_NAME "RhizoVision Explorer ${PROJECT_VERSION}")
set(CPACK_DMG_FORMAT "UDZO")
set(CPACK_PACKAGE_FILE_NAME "RhizoVisionExplorer-${PROJECT_VERSION}-macOS-arm64")
include(CPack)
