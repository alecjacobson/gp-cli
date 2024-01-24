if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG 8185a213d0314d2215995847cacc3b4c0ba8bbbc
)
FetchContent_MakeAvailable(libigl)
