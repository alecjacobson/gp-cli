cmake_minimum_required(VERSION 3.1)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)


# libigl
option(LIBIGL_USE_PREBUILT_LIBRARIES "Use prebuilt libraries"       ON)
option(LIBIGL_USE_STATIC_LIBRARY     "Use libigl as static library" ${LIBIGL_USE_PREBUILT_LIBRARIES})
option(LIBIGL_COPYLEFT_CGAL          "Use CGAL"                     ON)
option(LIBIGL_COMISO                 "Use CoMiso"                   OFF)
option(LIBIGL_CORK                   "Use Cork"                     OFF)
option(LIBIGL_EMBREE                 "Use Embree"                   ON)
option(LIBIGL_RESTRICTED_TETGEN      "Use Matlab"                   OFF)
option(LIBIGL_RESTRICTED_TETGEN      "Use MOSEK"                    OFF)
option(LIBIGL_OPENGL                 "Use OpenGL"                   ON)
option(LIBIGL_GLFW                   "Use GLFW"                     ON)
option(LIBIGL_IMGUI                  "Use ImGui"                    ON)
option(LIBIGL_PNG                    "Use PNG"                      ON)
option(LIBIGL_COPYLEFT_TETGEN        "Use Tetgen"                   ON)
option(LIBIGL_RESTRICTED_TETGEN      "Use Triangle"                 ON)
option(LIBIGL_RESTRICTED_TETGEN      "Use exact predicates"         OFF)
option(LIBIGL_XML                    "Use XML"                      ON)
find_package(LIBIGL REQUIRED QUIET)

add_executable(convertmesh convertmesh.cpp)
target_link_libraries(convertmesh igl::core igl::xml)
list(APPEND BIN_TARGETS "convertmesh")

add_executable(mergestl mergestl.cpp)
target_link_libraries(mergestl igl::core igl::xml)
list(APPEND BIN_TARGETS "mergestl")

if(LIBIGL_WITH_EMBREE)
  add_executable(reorient reorient.cpp)
  target_link_libraries(reorient igl::core igl::xml igl::embree)
  list(APPEND BIN_TARGETS "reorient")
endif()

add_executable(decimate decimate.cpp)
target_link_libraries(decimate igl::core igl::xml )
list(APPEND BIN_TARGETS "decimate")

add_executable(meshboolean meshboolean.cpp)
target_link_libraries(meshboolean igl::core igl::xml igl_copyleft::cgal)
list(APPEND BIN_TARGETS "meshboolean")

add_executable(meshstatistics meshstatistics.cpp)
target_link_libraries(meshstatistics igl::core igl::xml igl_copyleft::cgal)
list(APPEND BIN_TARGETS "meshstatistics")

add_executable(minkowski minkowski.cpp)
target_link_libraries(minkowski igl::core igl::xml igl_copyleft::cgal)
list(APPEND BIN_TARGETS "minkowski")

add_executable(resolvemesh resolvemesh.cpp)
target_link_libraries(resolvemesh igl::core igl_copyleft::cgal igl::xml)
list(APPEND BIN_TARGETS "resolvemesh")

add_executable(scrubmesh scrubmesh.cpp)
target_link_libraries(scrubmesh igl::core igl::opengl igl::glfw igl::png)
list(APPEND BIN_TARGETS "scrubmesh")

add_executable(upright upright.cpp)
target_link_libraries(upright igl::core igl::opengl igl::glfw igl::imgui)
list(APPEND BIN_TARGETS "upright")

add_executable(viewmesh viewmesh.cpp)
target_link_libraries(viewmesh igl::core igl::opengl igl::glfw igl::png)
list(APPEND BIN_TARGETS "viewmesh")

install(TARGETS ${BIN_TARGETS} DESTINATION ${CMAKE_INSTALL_PREFIX} RUNTIME DESTINATION "bin")
