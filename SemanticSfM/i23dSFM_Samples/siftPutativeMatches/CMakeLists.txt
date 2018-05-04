
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ADD_EXECUTABLE(i23dSFM_sample_siftPutative siftmatch.cpp)
TARGET_LINK_LIBRARIES(i23dSFM_sample_siftPutative
  i23dSFM_image
  i23dSFM_features
  i23dSFM_matching
  vlsift
  stlplus)

SET_PROPERTY(TARGET i23dSFM_sample_siftPutative PROPERTY FOLDER I23dSFM/Samples)
