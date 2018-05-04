INCLUDE_DIRECTORIES(SYSTEM)
FIND_PACKAGE(MPI)
FIND_PACKAGE(CUDA)
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
FIND_PACKAGE(OpenCV REQUIRED)

set(CMAKE_CXX_COMPILER "mpicxx")
set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(SERVER_FILE "hosts")
CONFIGURE_FILE(${SERVER_FILE} "${CMAKE_BINARY_DIR}/software/SfM/hosts.txt" )

add_executable(i23dSFM_main_mpiComputeFeatures main_mpiComputeFeatures_v2.cpp)
target_link_libraries(
        i23dSFM_main_mpiComputeFeatures
        i23dSFM_system
        i23dSFM_image
        i23dSFM_features
        i23dSFM_multiview
        i23dSFM_sfm
        stlplus
        vlsift
)



SET_PROPERTY(TARGET i23dSFM_main_mpiComputeFeatures PROPERTY FOLDER I23dSFM/software)
INSTALL(TARGETS i23dSFM_main_mpiComputeFeatures DESTINATION bin/)

include_directories(${I23dSFM}/matching_image_collection)

#
#add_executable(i23dSFM_main_mpiIncrementalSfM main_mpiIncrementalSfM.cpp)
#target_link_libraries(
#        i23dSFM_main_mipiComputeFeatures
#        i23dSFM_system
#        i23dSFM_image
#        i23dSFM_features
#        i23dSFM_multiview
#        i23dSFM_sfm
#        stlplus
#        vlsift
#        mpi_bundle_adjustment
#)
#
#
#SET_PROPERTY(TARGET i23dSFM_main_mipiComputeFeatures PROPERTY FOLDER I23dSFM/software)
#INSTALL(TARGETS i23dSFM_main_mipiComputeFeatures DESTINATION bin/)
#
#include_directories(${I23dSFM}/matching_image_collection)



#
#add_executable(i23dSFM_main_mpiComputeMatches
#        main_mpiComputeMatches.cpp
#
#        )
#
#target_link_libraries(
#        i23dSFM_main_mpiComputeMatches
#        i23dSFM_system
#        i23dSFM_image
#        i23dSFM_features
#        i23dSFM_multiview
#        i23dSFM_sfm
#        stlplus
#        vlsift
#)
#SET_PROPERTY(TARGET i23dSFM_main_mpiComputeMatches PROPERTY FOLDER I23dSFM/software)
#INSTALL(TARGETS i23dSFM_main_mpiComputeMatches DESTINATION bin/)





