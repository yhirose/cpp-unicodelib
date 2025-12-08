add_library(unicodelib_module)

target_sources(unicodelib_module
    PUBLIC
        FILE_SET CXX_MODULES FILES
            unicodelib.cppm
            unicodelib.names.cppm
)

target_compile_features(unicodelib_module PUBLIC cxx_std_20)

target_include_directories(unicodelib_module PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(unicodelib::module ALIAS unicodelib_module)

# Installation
install(TARGETS unicodelib_module
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/unicodelib/modules
)