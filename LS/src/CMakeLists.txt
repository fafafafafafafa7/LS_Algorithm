include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/LS/include)

aux_source_directory(. LS_SOURCE)

add_library(LS ${LS_SOURCE})