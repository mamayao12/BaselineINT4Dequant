file(GLOB BASELINEINT4DEQUANT_SOURCES "*.cpp")
file(GLOB BASELINEINT4DEQUANT_HEADERS "*.h")



# exclude files including tb_ from sources
list(FILTER BASELINEINT4DEQUANT_SOURCES EXCLUDE REGEX "testbench*")

# include_directories(../x_Shared)

add_executable(sim_INT4Dequant testbench.cpp ${BASELINEINT4DEQUANT_SOURCES} ${BASELINEINT4DEQUANT_HEADERS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CFLAGS} ${SIMFLAGS_CYCACC} ${LIBFLAGS}")