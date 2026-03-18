# Applications/CMakeLists.txt

# Shared include directories
include_directories(${CMAKE_SOURCE_DIR}/Common)
include_directories(${CMAKE_SOURCE_DIR}/ModelsHW)
include_directories(${CMAKE_SOURCE_DIR}/ModelsAlg)

# Add application subdirectories
add_subdirectory(DotProductINT8)
add_subdirectory(SystolicArrayINT8)
