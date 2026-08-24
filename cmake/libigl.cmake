if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG 0cbcaf886314ff331402c38fbb04cc4de8d7c033
)
FetchContent_MakeAvailable(libigl)
