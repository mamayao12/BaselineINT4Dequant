cmake_minimum_required(VERSION 3.22)
project(CATEXP)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_COLOR_DIAGNOSTICS ON)

message(STATUS "High Level Synthesis C Model Project")

cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)
message(STATUS "Hostname: ${HOSTNAME}")
message(STATUS "Processor architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")



if (HOSTNAME MATCHES "Ub22Ctp24Sixu")
    message(STATUS "Detected Sixu's MacOS Machine")
    set(CATAPULT_HOME "/Users/lisixu/Tools/catapult/2024.1.1")
    set(MATCHLIB_HOME "/Users/lisixu/Projects/EICHW/referencelibs/matchlib-20240726/cmod/include")
elseif(HOSTNAME MATCHES "eic-gt-cpu1")
    message(STATUS "Detected EIC lab CPU Machine")
    set(CATAPULT_HOME "/tools/software/siemens/catapult/2024.1_2")
    set(MATCHLIB_HOME "/media/WDS_4TB/PDLibs/RefLibs/matchlib-20240726/cmod/include")
    
else ()
    message(FATAL_ERROR "Unknown hostname, exit cmake, please add the hostname to the list and set the paths")
endif ()
message(STATUS "Catapult home set to: ${CATAPULT_HOME}")
message(STATUS "Matchlib home set to: ${MATCHLIB_HOME}")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    message(STATUS "Boost found")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

# set catapult compiler
set(CMAKE_C_COMPILER ${CATAPULT_HOME}/Mgc_home/pkgs/dcs_gcc/gcc-10.3.0/bin/gcc)
set(CMAKE_CXX_COMPILER ${CATAPULT_HOME}/Mgc_home/pkgs/dcs_gcc/gcc-10.3.0/bin/g++)

# matchlib nvlibs
include_directories(${MATCHLIB_HOME})

# catapult systemc libs
include_directories(${CATAPULT_HOME}/Mgc_home/shared/include)
link_directories(${CATAPULT_HOME}/Mgc_home/shared/lib)
link_libraries(${CATAPULT_HOME}/Mgc_home/shared/lib/libsystemc.a)

link_directories(${CATAPULT_HOME}/Mgc_home/pkgs/dcs_gcc/gcc-10.3.0/lib64)
link_libraries(${CATAPULT_HOME}/Mgc_home/pkgs/dcs_gcc/gcc-10.3.0/lib64/libstdc++.a)

# boost libs
include_directories(${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIBRARYDIR})
#link_libraries(${BOOST_LIBRARYDIR})

# shared compilation options
set(CFLAGS "-Wall -Wno-unknown-pragmas -DDEBUG_LEVEL=1")
set(LIBFLAGS "-lstdc++ -lsystemc -lm -lpthread -lboost_timer -lboost_chrono -lboost_system")
set(SIMFLAGS "-DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES -DCONN_RAND_STALL -DHLS_CATAPULT")
set(SIMFLAGS_CYCACC "-DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES -DHLS_CATAPULT")

# add subdirectories
add_subdirectory(BaselineINT4Dequant)

# add_subdirectory(1_SDHW_2)
# add_subdirectory(SDAcc)


