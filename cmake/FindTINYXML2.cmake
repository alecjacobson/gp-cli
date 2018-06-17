# - Try to find the TINYXML2 library
# Once done this will define
#
#  TINYXML2_FOUND - system has TINYXML2
#  TINYXML2_INCLUDE_DIR - the TINYXML2 include directory
#  TINYXML2_SOURCES - the TINYXML2 source files

FIND_PATH(TINYXML2_INCLUDE_DIR tinyxml2.h
   /usr/include
   /usr/local/include
   ${LIBIGL_INCLUDE_DIR}/../external/tinyxml2/
   ${PROJECT_SOURCE_DIR}/../libigl/external/tinyxml2/
   ${PROJECT_SOURCE_DIR}/../../external/tinyxml2/
   ${PROJECT_SOURCE_DIR}/../external/tinyxml2/
   ${PROJECT_SOURCE_DIR}/external/tinyxml2/
)

SET(TINYXML2_FOUND "NO")
IF (TINYXML2_INCLUDE_DIR)
	SET(TINYXML2_FOUND "YES")
ENDIF (TINYXML2_INCLUDE_DIR)

set(TINYXML2_SOURCES ${TINYXML2_INCLUDE_DIR}/tinyxml2.cpp)

if(TINYXML2_INCLUDE_DIR)
   message(STATUS "Found TINYXML2: ${TINYXML2_INCLUDE_DIR}")
else(TINYXML2_INCLUDE_DIR)
  if (NOT TINYXML2_FIND_QUIETLY)
    message(FATAL_ERROR "could NOT find TINYXML2")
  endif (NOT TINYXML2_FIND_QUIETLY)
endif(TINYXML2_INCLUDE_DIR)

MARK_AS_ADVANCED(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARIES TINYXML2_SOURCES)
