include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/Algorithms/include)

aux_source_directory(. Algorithms_SOURCE)

add_library(Algorithms ${Algorithms_SOURCE})