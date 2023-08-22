
find_path(LIBIGL_INCLUDE_DIR igl/readOBJ.h
    HINTS
        ${LIBIGL_DIR}
        ENV LIBIGL_DIR
    PATHS
        ${CMAKE_SOURCE_DIR}/../..
        ${CMAKE_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/libigl
        ${CMAKE_SOURCE_DIR}/../libigl
        ${CMAKE_SOURCE_DIR}/../../libigl
        /usr
        /usr/local
        /usr/local/igl/libigl
    PATH_SUFFIXES include
    REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBIGL
    "\nlibigl not found --- You can download it using:\n\tgit clone https://github.com/libigl/libigl.git ${CMAKE_SOURCE_DIR}/../libigl"
    LIBIGL_INCLUDE_DIR)
mark_as_advanced(LIBIGL_INCLUDE_DIR)

list(APPEND CMAKE_MODULE_PATH "${LIBIGL_INCLUDE_DIR}/../cmake")

if(LIBIGL_USE_PREBUILT_LIBRARIES)
  if(DEFINED LIBIGL_USE_STATIC_LIBRARY AND NOT LIBIGL_USE_STATIC_LIBRARY)
    message(FATAL_ERROR "LIBIGL_USE_PREBUILT_LIBRARIES=ON but LIBIGL_USE_STATIC_LIBRARY=OFF")
  endif()

  set(LIBIGL_DIR "${LIBIGL_INCLUDE_DIR}/..")
  set(LIBIGL_EXTERNAL_DIR "${LIBIGL_DIR}/external")
  if(NOT DEFINED LIBIGL_LIB_DIR)
    find_path(LIBIGL_LIB_DIR libigl.a libigl.so libigl.dll
      HINTS
        ${LIBIGL_LIB_DIR}
        ENV LIBIGL_LIB_DIR
      PATHS
        ${LIBIGL_DIR}/build/lib
        ${LIBIGL_DIR}/build/
      REQUIRED
    )
  endif()
  message(STATUS "Using prebuilt libigl libraries in ${LIBIGL_LIB_DIR}")
  
  # Eigen
  add_library(igl_eigen INTERFACE)
  target_include_directories(igl_eigen SYSTEM INTERFACE
    $<BUILD_INTERFACE:${LIBIGL_EXTERNAL_DIR}/eigen> $<INSTALL_INTERFACE:include>)
  set_property(TARGET igl_eigen PROPERTY EXPORT_NAME Eigen3::Eigen)
  add_library(Eigen3::Eigen ALIAS igl_eigen)
  
  # libigl.a 
  add_library(igl_core INTERFACE)
  target_include_directories(igl_core SYSTEM INTERFACE
    $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
  add_library(               igl::core ALIAS igl_core)
  target_link_libraries(     igl_core INTERFACE Eigen3::Eigen)
  target_compile_features(   igl_core INTERFACE cxx_std_11)
  target_compile_definitions(igl_core INTERFACE -DIGL_STATIC_LIBRARY)
  find_library(LIBIGL_core_LIBRARY NAMES igl HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
  target_link_libraries(     igl_core INTERFACE ${LIBIGL_core_LIBRARY})
  
  # CGAL
  if(LIBIGL_COPYLEFT_CGAL)
    set(CGAL_DIR "${LIBIGL_EXTERNAL_DIR}/cgal")
    set(BOOST_ROOT "${LIBIGL_EXTERNAL_DIR}/boost")
    find_package(CGAL CONFIG COMPONENTS Core PATHS ${CGAL_DIR} NO_DEFAULT_PATH REQUIRED)
    
    # libigl_copyleft_cgal.a
    add_library(igl_copyleft_cgal INTERFACE)
    target_include_directories(igl_copyleft_cgal SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    add_library(               igl_copyleft::cgal ALIAS igl_copyleft_cgal)
    target_link_libraries(     igl_copyleft_cgal INTERFACE CGAL::CGAL CGAL::CGAL_Core)
    find_library(LIBIGL_cgal_LIBRARY NAMES igl_copyleft_cgal igl_copyleft_cgal HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_copyleft_cgal INTERFACE ${LIBIGL_cgal_LIBRARY})
  endif()
  
  # Embree
  if(LIBIGL_EMBREE)
    set(EMBREE_LIB_DIRS ${LIBIGL_LIB_DIR}/../_deps/embree-build ${LIBIGL_LIB_DIR}/embree)
    find_path(EMBREE_INCLUDE_DIR embree3/rtcore.h PATHS ${LIBIGL_EXTERNAL_DIR}/embree/include REQUIRED)
    find_library(EMBREE_embree3_LIBRARY     NAMES embree3     HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    find_library(EMBREE_embree_avx2_LIBRARY NAMES embree_avx2 HINTS ${EMBREE_LIB_DIRS} ) # not required (won't exist for debug)
    if(NOT EMBREE_embree_avx2_LIBRARY)
      set(EMBREE_embree_avx2_LIBRARY "")
    endif()
    find_library(EMBREE_lexers_LIBRARY      NAMES lexers      HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    find_library(EMBREE_math_LIBRARY        NAMES math        HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    find_library(EMBREE_simd_LIBRARY        NAMES simd        HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    find_library(EMBREE_sys_LIBRARY         NAMES sys         HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    find_library(EMBREE_tasking_LIBRARY     NAMES tasking     HINTS ${EMBREE_LIB_DIRS} REQUIRED)
    set(EMBREE_LIBRARIES ${EMBREE_embree3_LIBRARY} ${EMBREE_embree_avx2_LIBRARY} ${EMBREE_lexers_LIBRARY} ${EMBREE_math_LIBRARY} ${EMBREE_simd_LIBRARY} ${EMBREE_sys_LIBRARY} ${EMBREE_tasking_LIBRARY})
    
    # libigl_embree.a
    add_library(igl_embree INTERFACE)
    target_include_directories(igl_embree SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_embree INTERFACE ${EMBREE_INCLUDE_DIR})
    add_library(               igl::embree ALIAS igl_embree)
    target_link_libraries(     igl_embree INTERFACE ${EMBREE_LIBRARIES})
    find_library(LIBIGL_embree_LIBRARY NAMES igl_embree HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_embree INTERFACE ${LIBIGL_embree_LIBRARY})
  endif()
  
  # MATLAB
  if(LIBIGL_RESTRICTED_TETGEN)
    find_package(Matlab REQUIRED COMPONENTS MEX_COMPILER MX_LIBRARY ENG_LIBRARY MAT_LIBRARY)
  
    # libigl_matlab.a
    add_library(igl_matlab INTERFACE)
    target_include_directories(igl_matlab SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_matlab INTERFACE ${Matlab_INCLUDE_DIRS})
    add_library(               igl::matlab ALIAS igl_matlab)
    target_link_libraries(     igl_matlab INTERFACE ${Matlab_LIBRARIES})
    find_library(LIBIGL_matlab_LIBRARY NAMES igl_matlab HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_matlab INTERFACE ${LIBIGL_matlab_LIBRARY})
  endif()
  
  # MOSEK
  if(LIBIGL_RESTRICTED_TETGEN)
    find_package(MOSEK REQUIRED)
  
    # libigl_mosek.a
    add_library(igl_mosek INTERFACE)
    target_include_directories(igl_mosek SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_mosek INTERFACE ${MOSEK_INCLUDE_DIRS})
    add_library(               igl::mosek ALIAS igl_mosek)
    target_link_libraries(     igl_mosek INTERFACE ${MOSEK_LIBRARIES})
    find_library(LIBIGL_mosek_LIBRARY NAMES igl_mosek HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_mosek INTERFACE ${LIBIGL_mosek_LIBRARY})
  endif()
  
  # OpenGL
  if(LIBIGL_OPENGL OR LIBIGL_GLFW OR LIBIGL_IMGUI)
    find_package(OpenGL REQUIRED)
  
    # Glad
    find_path(GLAD_INCLUDE_DIR glad/glad.h PATHS ${LIBIGL_EXTERNAL_DIR}/glad/include REQUIRED)
    find_library(GLAD_LIBRARY NAMES glad HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    
    # libigl_opengl.a
    add_library(igl_opengl INTERFACE)
    target_include_directories(igl_opengl SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_opengl INTERFACE ${OPENGL_INCLUDE_DIR} ${GLAD_INCLUDE_DIR})
    add_library(               igl::opengl ALIAS igl_opengl)
    target_link_libraries(     igl_opengl INTERFACE ${OPENGL_gl_LIBRARY} ${GLAD_LIBRARY})
    find_library(LIBIGL_opengl_LIBRARY NAMES igl_opengl HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_opengl INTERFACE ${LIBIGL_opengl_LIBRARY})
    
    # GLFW
    if(LIBIGL_GLFW OR LIBIGL_IMGUI)
      find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h PATHS ${LIBIGL_EXTERNAL_DIR}/glfw/include REQUIRED)
      find_library(glfw_LIBRARIES NAMES glfw3 HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
      list(APPEND glfw_LIBRARIES
          "-framework Cocoa"
          "-framework IOKit"
          "-framework CoreFoundation")
      
      
      # libigl_glfw.a
      add_library(igl_glfw INTERFACE)
      target_include_directories(igl_glfw SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
      target_include_directories(igl_glfw INTERFACE ${GLFW_INCLUDE_DIR})
      add_library(               igl::glfw ALIAS igl_glfw)
      target_link_libraries(     igl_glfw INTERFACE ${glfw_LIBRARIES})
      find_library(LIBIGL_glfw_LIBRARY NAMES igl_glfw HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
      target_link_libraries(     igl_glfw INTERFACE ${LIBIGL_glfw_LIBRARY})
    
      if(LIBIGL_IMGUI)
        # Imgui
        find_path(IMGUI_INCLUDE_DIR imgui/imgui.h PATHS ${LIBIGL_EXTERNAL_DIR}/ REQUIRED)
        find_path(LIBIGL_IMGUI_INCLUDE_DIR imgui_fonts_droid_sans.h PATHS ${LIBIGL_EXTERNAL_DIR}/libigl-imgui REQUIRED)
        set(IMGUI_INCLUDE_DIRS ${IMGUI_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR}/imgui/ ${IMGUI_INCLUDE_DIR}/imgui/examples ${LIBIGL_IMGUI_INCLUDE_DIR})
        find_library(IMGUI_LIBRARY NAMES imgui HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
        
        # Imguizmo
        find_path(IMGUIZMO_INCLUDE_DIR ImGuizmo.h PATHS ${LIBIGL_EXTERNAL_DIR}/imguizmo REQUIRED)
        find_library(IMGUIZMO_LIBRARY NAMES imguizmo HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
        
        # libigl_imgui.a
        add_library(igl_imgui INTERFACE)
        target_include_directories(igl_imgui SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
        target_include_directories(igl_imgui INTERFACE ${IMGUI_INCLUDE_DIRS} ${IMGUIZMO_INCLUDE_DIR})
        target_compile_definitions(igl_imgui INTERFACE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
        add_library(               igl::imgui ALIAS igl_imgui)
        target_link_libraries(     igl_imgui INTERFACE ${IMGUI_LIBRARY} ${IMGUIZMO_LIBRARY})
        find_library(LIBIGL_imgui_LIBRARY NAMES igl_imgui HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
        target_link_libraries(     igl_imgui INTERFACE ${LIBIGL_imgui_LIBRARY})
      endif()
    endif()
  endif()
  
  # stb_image
  if(LIBIGL_PNG)
    find_path(STB_IMAGE_INCLUDE_DIR igl_stb_image.h PATHS ${LIBIGL_EXTERNAL_DIR}/stb REQUIRED)
  
    # libigl_png.a
    add_library(igl_png INTERFACE)
    target_include_directories(igl_png SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_png INTERFACE ${STB_IMAGE_INCLUDE_DIR})
    add_library(               igl::png ALIAS igl_png)
    find_library(LIBIGL_png_LIBRARY NAMES igl_png HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    find_library(LIBIGL_stb_image_LIBRARY NAMES stb HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_png INTERFACE ${LIBIGL_png_LIBRARY} ${LIBIGL_stb_image_LIBRARY})
    target_link_libraries(     igl_png INTERFACE ${LIBIGL_png_LIBRARY} )
  endif()
  
  # Tetgen
  if(LIBIGL_COPYLEFT_TETGEN)
    find_path(TETGEN_INCLUDE_DIR tetgen.h PATHS ${LIBIGL_EXTERNAL_DIR}/tetgen/ REQUIRED)
    find_library(TETGEN_LIBRARY NAMES tetgen HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
  
    # libigl_tetgen.a
    add_library(igl_copyleft_tetgen INTERFACE)
    target_include_directories(igl_copyleft_tetgen SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_copyleft_tetgen INTERFACE ${TETGEN_INCLUDE_DIR})
    add_library(               igl::tetgen ALIAS igl_copyleft_tetgen)
    target_link_libraries(     igl_copyleft_tetgen INTERFACE ${TETGEN_LIBRARY})
    find_library(LIBIGL_tetgen_LIBRARY NAMES igl_copyleft_tetgen HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_copyleft_tetgen INTERFACE ${LIBIGL_tetgen_LIBRARY})
  endif()
    
  # Triangle
  if(LIBIGL_RESTRICTED_TETGEN)
    find_path(TRIANGLE_INCLUDE_DIR triangle.h PATHS ${LIBIGL_EXTERNAL_DIR}/triangle/ REQUIRED)
    find_library(TRIANGLE_LIBRARY NAMES triangle HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    
    # libigl_restricted_triangle.a
    add_library(igl_restricted_triangle INTERFACE)
    target_include_directories(igl_restricted_triangle SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_restricted_triangle INTERFACE ${TRIANGLE_INCLUDE_DIR})
    add_library(               igl_restricted::triangle ALIAS igl_restricted_triangle)
    target_link_libraries(     igl_restricted_triangle INTERFACE ${TRIANGLE_LIBRARY})
    find_library(LIBIGL_triangle_LIBRARY NAMES igl_restricted_triangle HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_restricted_triangle INTERFACE ${LIBIGL_triangle_LIBRARY})
  endif()
  
  
  # TinyXML2
  if(LIBIGL_XML)
    find_path(TINYXML2_INCLUDE_DIR tinyxml2.h PATHS ${LIBIGL_EXTERNAL_DIR}/tinyxml2/ REQUIRED)
    find_library(TINYXML2_LIBRARY NAMES tinyxml2 HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
  
    # libigl_xml.a
    add_library(igl_xml INTERFACE)
    target_include_directories(igl_xml SYSTEM INTERFACE $<BUILD_INTERFACE:${LIBIGL_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
    target_include_directories(igl_xml INTERFACE ${TINYXML2_INCLUDE_DIR})
    add_library(               igl::xml ALIAS igl_xml)
    target_link_libraries(     igl_xml INTERFACE ${TINYXML2_LIBRARY})
    find_library(LIBIGL_tinyxml2_LIBRARY NAMES igl_xml HINTS  ${LIBIGL_LIB_DIR} REQUIRED)
    target_link_libraries(     igl_xml INTERFACE ${LIBIGL_tinyxml2_LIBRARY})
  endif()

else()
  include(libigl)
endif()
