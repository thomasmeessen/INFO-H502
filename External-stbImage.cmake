message( "External project - STBImage" )

ExternalProject_Add(stbImage
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        SOURCE_DIR stb_source
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ""
        )

#----
# Matching the Cmake module FindGtest output variable name to seamlessly switch between the two
# https://cmake.org/cmake/help/latest/module/FindGTest.html

set( STB_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/stb_source/ )
