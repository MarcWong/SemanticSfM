
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(i23dSFM_sample_robustEssential robust_essential.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_robustEssential
  i23dSFM_image
  i23dSFM_multiview
  i23dSFM_system
  i23dSFM_features
  i23dSFM_matching
  i23dSFM_sfm
  vlsift)

SET_PROPERTY(TARGET i23dSFM_sample_robustEssential PROPERTY FOLDER I23dSFM/Samples)
