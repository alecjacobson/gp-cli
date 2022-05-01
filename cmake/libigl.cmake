if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG 91f6c503f62d16f4d27709ea5be0d9ab43611942
)
FetchContent_MakeAvailable(libigl)
