
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(i23dSFM_sample_robustHomography_guided robust_homography_guided.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_robustHomography_guided 
  i23dSFM_image
  i23dSFM_multiview
  i23dSFM_features
  i23dSFM_matching
  vlsift
  stlplus)

SET_PROPERTY(TARGET i23dSFM_sample_robustHomography_guided PROPERTY FOLDER I23dSFM/Samples)
