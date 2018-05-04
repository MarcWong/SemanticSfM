
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(i23dSFM_sample_robustFundamental robust_fundamental.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_robustFundamental
  i23dSFM_image
  i23dSFM_multiview
  i23dSFM_features
  i23dSFM_matching
  stlplus
  vlsift)

SET_PROPERTY(TARGET i23dSFM_sample_robustFundamental PROPERTY FOLDER I23dSFM/Samples)
