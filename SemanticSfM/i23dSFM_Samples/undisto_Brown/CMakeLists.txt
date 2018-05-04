
ADD_EXECUTABLE(i23dSFM_sample_undistoBrown undistoBrown.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_undistoBrown
  i23dSFM_image
  i23dSFM_multiview
  stlplus
  ${CERES_LIBRARIES})
SET_PROPERTY(TARGET i23dSFM_sample_undistoBrown PROPERTY FOLDER I23dSFM/Samples)
