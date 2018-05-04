
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(i23dSFM_enft_matching enft_matching.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_enft_matching
  i23dSFM_system
  i23dSFM_image
  i23dSFM_sfm
  i23dSFM_multiview
  i23dSFM_features
  i23dSFM_matching
  vlsift
  stlplus)
SET_PROPERTY(TARGET i23dSFM_enft_matching PROPERTY FOLDER I23dSFM/Samples)
