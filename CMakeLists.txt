project(SAPHRON)
cmake_minimum_required(VERSION 2.8.5)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(SAPH_USE_LINAL false)
SET(ARMA_PATHS "" CACHE STRING "Search directory for Armadillo")

# BEGIN - Identify linear algebra dependencies.
include(FindArma)
include(FindQuadMath)
include(FindgFortran)
include(FindBLAS)
include(FindLAPACK)
include(FindMKL)

message(STATUS "      ARMA_FOUND = ${ARMA_FOUND}"  )
message(STATUS "  QUADMATH_FOUND = ${QUADMATH_FOUND}"  )
message(STATUS "  GFORTRAN_FOUND = ${GFORTRAN_FOUND}"  )
message(STATUS "      BLAS_FOUND = ${BLAS_FOUND}"    )
message(STATUS "    LAPACK_FOUND = ${LAPACK_FOUND}"  )
message(STATUS "       MKL_FOUND = ${MKL_FOUND}"     )

if(MKL_FOUND)
	set(SAPH_LIBS ${SAPH_LIBS} ${MKL_LIBRARIES})
elseif(LAPACK_FOUND)
	set(SAPH_LIBS ${SAPH_LIBS} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${GFORTRAN_LIBRARIES} ${QUADMATH_LIBRARIES})
endif()

message(STATUS "")
message(STATUS "*** SAPHRON will use the following libraries:")
message(STATUS "*** SAPH_LIBS = ${SAPH_LIBS}")
message(STATUS "")
# END - Identify linear algebra dependencies.

# Find Google Test Framework 
include(ExternalProject)
 
# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/extern)
 
# Add gtest
# http://stackoverflow.com/questions/9689183/cmake-googletest
ExternalProject_Add(
	googletest
	URL http://googletest.googlecode.com/files/gtest-1.6.0.zip
	# TIMEOUT 10
	# # Force separate output paths for debug and release builds to allow easy
	# # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
	# CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
	# -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
	# -Dgtest_force_shared_crt=ON
	# Disable install step
	INSTALL_COMMAND ""
	# Wrap download, configure and build steps in a script to log output
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
)
 
# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include)

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
set(GTEST_LIBRARY_MAIN_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a)
set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION
${GTEST_LIBRARY_PATH} )
add_dependencies(${GTEST_LIBRARY} googletest) 

message(STATUS "")
message(STATUS "*** Google Test Framework will be used for unit tests")
message(STATUS "*** GTEST_LIBRARY_PATH = ${GTEST_LIBRARY_PATH}")
message(STATUS "*** GTEST_INCLUDE_DIR  = ${GTEST_INCLUDE_DIR}")
message(STATUS "")

# End gtest

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${ARMA_INCLUDE}")

set(SOURCE
	src/Ensembles/Ensemble.cpp
	src/Ensembles/DOSEnsemble.cpp
	src/Ensembles/NVTEnsemble.cpp
	src/ForceFields/ForceFieldManager.cpp
	src/Particles/Particle.cpp
	src/Observers/CSVObserver.cpp
	src/Observers/ConsoleObserver.cpp
	src/Observers/JSONObserver.cpp
	src/Simulation/SimObservable.cpp
	src/Simulation/SimObserver.cpp
	src/Worlds/SimpleWorld.cpp
	src/jsoncpp.cpp 
)

add_library(libsaphron ${SOURCE})
set_target_properties(libsaphron PROPERTIES PREFIX "")
target_link_libraries(libsaphron ${SAPH_LIBS})

add_executable(saphron 
	src/main.cpp 
	src/Simulation/SimBuilder.cpp
)
target_link_libraries(saphron ${SAPH_LIBS} libsaphron)

# Configure flags 
# - Globals 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fopenmp -pthread -std=c++11 -include ${PROJECT_SOURCE_DIR}/include/gcc-preinclude.h")
set_target_properties(libsaphron saphron PROPERTIES COMPILE_FLAGS "-Wall -Wextra")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed -fopenmp")

# Add testing 
enable_testing()
set(TEST_DEPS 
	${SAPH_LIBS} 
	${GTEST_LIBRARY_PATH} 
	${GTEST_LIBRARY_MAIN_PATH}
	libsaphron)

add_executable(DOSEnsembleTests test/DOSEnsembleTests.cpp)
target_link_libraries(DOSEnsembleTests ${TEST_DEPS})
target_include_directories(DOSEnsembleTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DOSEnsembleTests googletest) 
add_test(DOSEnsembleTests DOSEnsembleTests)

add_executable(DLSAConnectivityTests test/DLSAConnectivityTests.cpp)
target_link_libraries(DLSAConnectivityTests ${TEST_DEPS})
target_include_directories(DLSAConnectivityTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DLSAConnectivityTests googletest) 
add_test(DLSAConnectivityTests DLSAConnectivityTests)

add_executable(ElasticCoeffOPTests test/ElasticCoeffOPTests.cpp)
target_link_libraries(ElasticCoeffOPTests ${TEST_DEPS})
target_include_directories(ElasticCoeffOPTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ElasticCoeffOPTests googletest) 
add_test(ElasticCoeffOPTests ElasticCoeffOPTests)

add_executable(FlipSpinMoveTests test/FlipSpinMoveTests.cpp)
target_link_libraries(FlipSpinMoveTests ${TEST_DEPS})
target_include_directories(FlipSpinMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(FlipSpinMoveTests googletest) 
add_test(FlipSpinMoveTests FlipSpinMoveTests)

add_executable(ForceFieldManagerTests test/ForceFieldManagerTests.cpp)
target_link_libraries(ForceFieldManagerTests ${TEST_DEPS})
target_include_directories(ForceFieldManagerTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ForceFieldManagerTests googletest) 
add_test(ForceFieldManagerTests ForceFieldManagerTests)

add_executable(HistogramTests test/HistogramTests.cpp)
target_link_libraries(HistogramTests ${TEST_DEPS})
target_include_directories(HistogramTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(HistogramTests googletest) 
add_test(HistogramTests HistogramTests)

add_executable(IdentityChangeMoveTests test/IdentityChangeMoveTests.cpp)
target_link_libraries(IdentityChangeMoveTests ${TEST_DEPS})
target_include_directories(IdentityChangeMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(IdentityChangeMoveTests googletest) 
add_test(IdentityChangeMoveTests IdentityChangeMoveTests)

add_executable(LebwohlLasherFFTests test/LebwohlLasherFFTests.cpp)
target_link_libraries(LebwohlLasherFFTests ${TEST_DEPS})
target_include_directories(LebwohlLasherFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(LebwohlLasherFFTests googletest) 
add_test(LebwohlLasherFFTests LebwohlLasherFFTests)

add_executable(NVTEnsembleTests test/NVTEnsembleTests.cpp)
target_link_libraries(NVTEnsembleTests ${TEST_DEPS})
target_include_directories(NVTEnsembleTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(NVTEnsembleTests googletest) 
add_test(NVTEnsembleTests NVTEnsembleTests)

add_executable(P2SAConnectivityTests test/P2SAConnectivityTests.cpp)
target_link_libraries(P2SAConnectivityTests ${TEST_DEPS})
target_include_directories(P2SAConnectivityTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(P2SAConnectivityTests googletest) 
add_test(P2SAConnectivityTests P2SAConnectivityTests)

add_executable(P2SFAConnectivityTests test/P2SFAConnectivityTests.cpp)
target_link_libraries(P2SFAConnectivityTests ${TEST_DEPS})
target_include_directories(P2SFAConnectivityTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(P2SFAConnectivityTests googletest) 
add_test(P2SFAConnectivityTests P2SFAConnectivityTests)

add_executable(ParticleObserverTests test/ParticleObserverTests.cpp)
target_link_libraries(ParticleObserverTests ${TEST_DEPS})
target_include_directories(ParticleObserverTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ParticleObserverTests googletest) 
add_test(ParticleObserverTests ParticleObserverTests)

add_executable(RandTests test/RandTests.cpp)
target_link_libraries(RandTests ${TEST_DEPS})
target_include_directories(RandTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(RandTests googletest) 
add_test(RandTests RandTests)

add_executable(SimFlagsTests test/SimFlagsTests.cpp)
target_link_libraries(SimFlagsTests ${TEST_DEPS})
target_include_directories(SimFlagsTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(SimFlagsTests googletest) 
add_test(SimFlagsTests SimFlagsTests)

add_executable(SimpleWorldTests test/SimpleWorldTests.cpp)
target_link_libraries(SimpleWorldTests ${TEST_DEPS})
target_include_directories(SimpleWorldTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(SimpleWorldTests googletest) 
add_test(SimpleWorldTests SimpleWorldTests)

add_executable(SiteTests test/SiteTests.cpp)
target_link_libraries(SiteTests ${TEST_DEPS})
target_include_directories(SiteTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(SiteTests googletest) 
add_test(SiteTests SiteTests)

add_executable(SpeciesSwapMoveTests test/SpeciesSwapMoveTests.cpp)
target_link_libraries(SpeciesSwapMoveTests ${TEST_DEPS})
target_include_directories(SpeciesSwapMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(SpeciesSwapMoveTests googletest) 
add_test(SpeciesSwapMoveTests SpeciesSwapMoveTests)

add_executable(SphereUnitVectorMoveTests test/SphereUnitVectorMoveTests.cpp)
target_link_libraries(SphereUnitVectorMoveTests ${TEST_DEPS})
target_include_directories(SphereUnitVectorMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(SphereUnitVectorMoveTests googletest) 
add_test(SphereUnitVectorMoveTests SphereUnitVectorMoveTests)

add_executable(LennardJonesFFTests test/LennardJonesFFTests.cpp)
target_link_libraries(LennardJonesFFTests ${TEST_DEPS})
target_include_directories(LennardJonesFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(LennardJonesFFTests googletest) 
add_test(LennardJonesFFTests LennardJonesFFTests)

add_executable(TranslateMoveTests test/TranslateMoveTests.cpp)
target_link_libraries(TranslateMoveTests ${TEST_DEPS})
target_include_directories(TranslateMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(TranslateMoveTests googletest) 
add_test(TranslateMoveTests TranslateMoveTests)

add_executable(VolumeScaleMoveTests test/VolumeScaleMoveTests.cpp)
target_link_libraries(VolumeScaleMoveTests ${TEST_DEPS})
target_include_directories(VolumeScaleMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(VolumeScaleMoveTests googletest) 
add_test(VolumeScaleMoveTests VolumeScaleMoveTests)

add_executable(ParticleSwapMoveTests test/ParticleSwapMoveTests.cpp)
target_link_libraries(ParticleSwapMoveTests ${TEST_DEPS})
target_include_directories(ParticleSwapMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ParticleSwapMoveTests googletest) 
add_test(ParticleSwapMoveTests ParticleSwapMoveTests)

# end add testing 

install(TARGETS saphron libsaphron 
	RUNTIME DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
	ARCHIVE DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

message(STATUS "CMAKE_CXX_FLAGS           = ${CMAKE_CXX_FLAGS}"          )
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "CMAKE_REQUIRED_INCLUDES   = ${CMAKE_REQUIRED_INCLUDES}"  )
