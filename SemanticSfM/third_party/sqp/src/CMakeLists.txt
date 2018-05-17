add_executable(test_fmincon test_fmincon.cpp)
target_link_libraries(test_fmincon 
		     ${PROJECT_SOURCE_DIR}/libFmincon.so
		     /usr/local/MATLAB/R2016b/runtime/glnxa64/libmwmclmcrrt.so.9.1
		     /usr/local/MATLAB/R2016b/bin/glnxa64/libmwmclmcr.so)
