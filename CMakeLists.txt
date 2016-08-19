# CMakeLists for MFA
# A. J. Smith (ajs224@cam.ac.uk)

# Build Release with: cmake -DCMAKE_BUILD_TYPE=Release
# Build Debug with: cmake -DCMAKE_BUILD_TYPE=Debug
# Check compile options with: make -j 8 VERBOSE=1

# Specify the version being used
cmake_minimum_required(VERSION 2.8)

# Specify language
enable_language(CXX)

# Project Name
set(PROJECT_NAME "mfa-c++")
MESSAGE("Project name: ${PROJECT_NAME}")
project(${PROJECT_NAME})
MESSAGE("Project Build: ${CMAKE_BUILD_TYPE}")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
 message(STATUS "Set build type to 'Debug' as none was specified.")
 set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
     "MinSizeRel" "RelWithDebInfo")
endif()


# Set binary path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Find BOOST and required components
#find_package(Boost 1.58.0 REQUIRED COMPONENTS mpi regex serialization filesystem system program_options)
find_package(Boost 1.58.0 REQUIRED COMPONENTS mpi serialization )
IF(Boost_FOUND)
    MESSAGE("Boost includes located in: ${Boost_INCLUDE_DIRS}")
    MESSAGE("Boost libraries located in: ${Boost_LIBRARIES}")
ENDIF(Boost_FOUND)

# Find MPI
find_package(MPI REQUIRED)
if ( MPI_FOUND )
   MESSAGE("MPI includes located in: ${MPI_INCLUDE_PATH}")
   MESSAGE("MPI libraries located in: ${MPI_LIBRARIES}")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_COMPILE_FLAGS} -lmpi  -L/opt/mpich/mpich-3.1.4/lib/") # -lpmi
endif( MPI_FOUND )
include_directories( ${MPI_INCLUDE_PATH} )

# ... or find OpenMP
#INCLUDE(FindOpenMP)
#IF(OPENMP_FOUND)
#  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#ENDIF(OPENMP_FOUND)


# Enable testing
enable_testing()

# Turn off line breaks in error messages so that IDEs can parse the errors properly
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Add compiler flags for Debug and Release
IF(CMAKE_COMPILER_IS_GNUCXX)

  # Debug mode		
  # Add -pg to the compile and link flags to enable profiling
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra -Weffc++ -Wno-format-errors")
  SET(CMAKE_LINK_FLAGS_DEBUG "${CMAKE_LINK_FLAGS_DEBUG}")

  # Release mode - with optimisation
  #SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -flto -fwhole-program -mtune=native")
  #SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG") # Basic O3
  
  # These optimisations seem to be faster with most of my code than the default release
  SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -flto -fwhole-program -mtune=native -DNDEBUG") 
  SET(CMAKE_LINK_FLAGS_RELEASE "${CMAKE_LINK_FLAGS_RELEASE}")
 
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorizer-verbose=1") # Tell whether loops were vectorized

ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# Set local include directory
include_directories(include)

######### The  application ##########################
add_executable(${PROJECT_NAME}
				 source/mfa.c++
				 #source/kernel.c++ 			 
				 source/mfa_params.c++
				 source/m_in.c++
				 source/moments.c++
				 source/parse_args.c++
				 source/Particle.c++
				 source/random.c++
				 source/theta.c++
				 )
# Make sure we're invoking compiler with -std=c++11
target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type cxx_range_for cxx_lambdas)


# Debug cmake
#MESSAGE("MPI_COMPILE_FLAGS: ${MPI_COMPILE_FLAGS}")
#MESSAGE("MPI_LINK_FLAGS: ${MPI_LINK_FLAGS}")
#MESSAGE("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
#MESSAGE("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")


# All the header files we have written
include_directories(${Boost_INCLUDE_DIRS}
                    include)
link_directories(${Boost_LIBRARIES} ${Boost_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME}  ${Boost_LIBRARIES} )


# Tests

# Does the application run?
#add_test (SolversRuns bin/pbIterativeSolver)
 
# Does the it run with constant kernel in the multicell case?
#add_test(NAME ConstantKernelMulticell
#            WORKING_DIRECTORY /tmp/pbIterativeSolver
#            COMMAND build/bin/pbIterativeSolver -cells 100 -length 1 -u 1 -k #constant -p 12 -res 1e-12 -mass -nin mono)

#set_tests_properties (ConstantKernelMulticell 
#  PROPERTIES PASS_REGULAR_EXPRESSION "100	    1.0000000000e+00	#6.0000000000e+00	4.6060000000e+01	4.5804360000e+02	9		#3.9790393203e-13")

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
