if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG 2869f9862926ef6e6acc5e2fa0b51f8fd165e22f
)
FetchContent_MakeAvailable(libigl)
