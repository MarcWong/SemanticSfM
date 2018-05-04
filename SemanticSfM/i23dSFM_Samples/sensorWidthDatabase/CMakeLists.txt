
# - Parse database
ADD_EXECUTABLE(i23dSFM_main_ParseDatabase main_ParseDatabase.cpp )
TARGET_LINK_LIBRARIES(i23dSFM_main_ParseDatabase
                     stlplus)
SET_PROPERTY(TARGET i23dSFM_main_ParseDatabase PROPERTY FOLDER I23dSFM/Samples)

