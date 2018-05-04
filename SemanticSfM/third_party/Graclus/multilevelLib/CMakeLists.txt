file(GLOB source . "wkkm.*" "mlkkm.*")

include_directories(${PROJECT_SOURCE_DIR}/metisLib)
add_library(multilevel SHARED ${source})
