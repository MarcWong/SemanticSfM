
#
# OpenGL
#
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
add_definitions(-DGL_GLEXT_PROTOTYPES -DGL3_PROTOTYPES)

if(APPLE)
  add_definitions(-DGLFW_INCLUDE_GLCOREARB)
endif()

#
# GLFW
#
if(!APPLE)
  add_definitions(-DGLFW_NO_GLU)
endif()

FIND_FILE(GL3HEADER gl3.h)
if(NOT ${GL3HEADER} MATCHES NOTFOUND)
add_definitions(-DGLFW_INCLUDE_GL3)
endif()

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
)

SET(SOURCES
  main.cpp
)

ADD_EXECUTABLE(i23dSFM_main_sfmViewer ${SOURCES} )
TARGET_LINK_LIBRARIES(i23dSFM_main_sfmViewer
  i23dSFM_image
  i23dSFM_features
  i23dSFM_multiview
  i23dSFM_sfm
  ${OPENGL_gl_LIBRARY}
  glfw
  ${GLFW_LIBRARIES})

SET_PROPERTY(TARGET i23dSFM_main_sfmViewer PROPERTY FOLDER I23dSFM/software)
INSTALL(TARGETS i23dSFM_main_sfmViewer DESTINATION bin/)
