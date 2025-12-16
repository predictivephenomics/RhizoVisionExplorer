
# We copy runtime dependent libraries for the RhizoVisionExplorer executable
# only for Windows. For linux we expect the runtime dependencies exist
# based on the dependencies mentioned while packaging.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach(opencv_lib ${OpenCV_LIBS})
        install(FILES $<TARGET_FILE:${opencv_lib}> CONFIGURATIONS Debug Release DESTINATION .)
    endforeach()

    # foreach(opencv_lib ${OpenCV_LIBS})
    #     install(FILES $<TARGET_FILE:${opencv_lib}> CONFIGURATIONS Release DESTINATION Release)
    # endforeach()

    foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
        install(FILES $<TARGET_FILE:${qt_lib}> CONFIGURATIONS Debug Release DESTINATION .)
    endforeach()

    # foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
    #     install(FILES $<TARGET_FILE:${qt_lib}> CONFIGURATIONS Release DESTINATION Release)
    # endforeach()

    # Install PDB files for OpenCV libraries
    foreach(opencv_lib ${OpenCV_LIBS})
        get_target_property(pdb_path ${opencv_lib} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${opencv_lib} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION .)
        endif()
    endforeach()

    # Install PDB files for Qt6 libraries
    foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
        get_target_property(pdb_path ${qt_lib} IMPORTED_PDB_LOCATION_DEBUG)
        if (NOT pdb_path)
            get_target_property(pdb_path ${qt_lib} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION .)
        endif()
    endforeach()

    # Install Qt6 support plugins
    foreach(qt_plugin Qt6::QWindowsIntegrationPlugin)
        install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Debug Release DESTINATION platforms)
        # install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Release DESTINATION Release/platforms)
        # Copy pdb files for the plugins
        get_target_property(pdb_path ${qt_plugin} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${qt_plugin} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION platforms)
        endif()
    endforeach()

    foreach(qt_plugin Qt6::QModernWindowsStylePlugin)
        install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Debug Release DESTINATION styles)
        # install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Release DESTINATION Release/styles)
        # Copy pdb files for the plugins
        get_target_property(pdb_path ${qt_plugin} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${qt_plugin} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION styles)
        endif()
    endforeach()

    # Install cvutil libraries
    foreach(cvutil_lib cvutil::cvutil cvutil::PluginManager cvutil::RoiManager)
        install(FILES $<TARGET_FILE:${cvutil_lib}> CONFIGURATIONS Debug Release DESTINATION .)
    endforeach()

    # Install cvutil PDB files
    foreach(cvutil_lib cvutil::cvutil cvutil::PluginManager cvutil::RoiManager)
        get_target_property(pdb_path ${cvutil_lib} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${cvutil_lib} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION .)
        endif()
    endforeach()

    # Install PDB files
    if(MSVC)
        install(FILES $<TARGET_PDB_FILE:RhizoVisionExplorer> CONFIGURATIONS Debug DESTINATION .)
    endif()
    
    # For copying runtime dependencies for RhizoVisionExplorer
    add_custom_command(TARGET RhizoVisionExplorer POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -D TARGET_FILE=$<TARGET_FILE:RhizoVisionExplorer>
    -D INSTALL_BIN=${CMAKE_INSTALL_PREFIX}
    -D SEARCH_DIRS=${DEP_PATH}
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/copy_runtime_deps.cmake"
    COMMENT "Copying runtime dependencies for cvutil"
    )

    install_cvutil_runtime_dependencies("${CMAKE_INSTALL_PREFIX}")
    
    # Install the RhizoVisionExplorer executable
    install(TARGETS RhizoVisionExplorer rv
    CONFIGURATIONS Debug Release
    RUNTIME DESTINATION .
    )

    # Install additional files.
    install(FILES README.md COPYING CONFIGURATIONS Debug Release DESTINATION .)

    install(FILES 
    licenses/LICENSE_Qt6
    licenses/LICENSE_opencv.txt
    licenses/LICENSE_cvutil
    CONFIGURATIONS Debug Release
    DESTINATION licenses
    )

    install(FILES manual/RhizoVisionExplorerManualv2.pdf CONFIGURATIONS Debug Release DESTINATION manual)

    install(FILES 
    imageexamples/crowns/crown1.png
    imageexamples/crowns/crown2.png
    imageexamples/crowns/crown3.png
    imageexamples/crowns/wheatcrown_settings.csv
    CONFIGURATIONS Debug Release
    DESTINATION imageexamples/crowns
    )

    install(FILES 
    imageexamples/scans/scan1.jpg
    imageexamples/scans/scan2.jpg
    imageexamples/scans/scan3.jpg
    imageexamples/scans/wheatscan_settings.csv
    CONFIGURATIONS Debug Release
    DESTINATION imageexamples/scans
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Install the RhizoVisionExplorer executable
    if(Qt6_FOUND)
        install(TARGETS RhizoVisionExplorer rv
            CONFIGURATIONS Debug Release
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    else()
        install(TARGETS rv
            CONFIGURATIONS Debug Release
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()

    # Install additional files.
    install(FILES README.md COPYING CONFIGURATIONS Debug Release DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/RhizoVisionExplorer)

    install(FILES 
        licenses/LICENSE_Qt6
        licenses/LICENSE_opencv.txt
        licenses/LICENSE_FFMPEG.txt
        licenses/LICENSE_cvutil
        licenses/LICENSE.indicators
        licenses/LICENSE.termcolor
        CONFIGURATIONS Debug Release
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/RhizoVisionExplorer/licenses
    )

    install(FILES manual/RhizoVisionExplorerManualv2.pdf CONFIGURATIONS Debug Release DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/RhizoVisionExplorer/manual)

    install(FILES 
        imageexamples/crowns/crown1.png
        imageexamples/crowns/crown2.png
        imageexamples/crowns/crown3.png
        imageexamples/crowns/wheatcrown_settings.csv
        CONFIGURATIONS Debug Release
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/RhizoVisionExplorer/imageexamples/crowns
    )

    install(FILES 
        imageexamples/scans/scan1.jpg
        imageexamples/scans/scan2.jpg
        imageexamples/scans/scan3.jpg
        imageexamples/scans/wheatscan_settings.csv
        CONFIGURATIONS Debug Release
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/RhizoVisionExplorer/imageexamples/scans
    )
endif()
