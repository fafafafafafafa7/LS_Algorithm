include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/Algorithms/include)
include_directories(${PROJECT_SOURCE_DIR}/LS/include)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(. Main_SOURCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(GrEdPa ${Main_SOURCE})

target_link_libraries(GrEdPa Algorithms)
target_link_libraries(GrEdPa LS)
