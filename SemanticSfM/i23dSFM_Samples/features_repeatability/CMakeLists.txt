
ADD_EXECUTABLE(i23dSFM_sample_main_repeatability_dataset main_repeatability_dataset.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_main_repeatability_dataset
  i23dSFM_image
  i23dSFM_features
  i23dSFM_matching
  i23dSFM_system
  i23dSFM_multiview
  vlsift
  stlplus)

SET_PROPERTY(TARGET i23dSFM_sample_main_repeatability_dataset PROPERTY FOLDER I23dSFM/Samples)
