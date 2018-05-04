if(I23dSFM_USE_OPENCV)
    add_definitions(-DTHIS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_executable( opencv_fitting main.cpp )
    target_link_libraries( opencv_fitting ${OpenCV_LIBS} i23dSFM_multiview) 
    install(TARGETS opencv_fitting DESTINATION bin/)
endif(I23dSFM_USE_OPENCV)


