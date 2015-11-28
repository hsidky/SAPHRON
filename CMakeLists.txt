project(SAPHRON)
cmake_minimum_required(VERSION 2.8.5)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# SAPHRON version number.
set(SAPHRON_VERSION_MAJOR "0")
set(SAPHRON_VERSION_MINOR "0")
set(SAPHRON_VERSION_TINY "7")  

# Default type is release
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

# Default is openmp.
set(SAPHRON_OPENMP true CACHE BOOL "Enable OpenMP parallelization")

set(SAPH_USE_LINAL false)
SET(ARMA_PATHS "" CACHE STRING "Search directory for Armadillo")

# BEGIN - Identify linear algebra dependencies.
include(FindArma)
#include(FindQuadMath)
#include(FindgFortran)
include(FindBLAS)
include(FindLAPACK)
include(FindMKL)

message(STATUS "      ARMA_FOUND = ${ARMA_FOUND}"  )
#message(STATUS "  QUADMATH_FOUND = ${QUADMATH_FOUND}"  )
#message(STATUS "  GFORTRAN_FOUND = ${GFORTRAN_FOUND}"  )
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

# Get Git Information. Taken from OpenMD.
find_package(Git)
if(GIT_FOUND) 
  include(GetGitRevisionDescription)
  get_git_head_revision(GIT_REFSPEC GIT_SHA1)
  MESSAGE(STATUS "Current revision is ${GIT_SHA1}")
  macro(make_release_tarball targetname releasename)
    add_custom_target(${targetname} COMMAND ${GIT_EXECUTABLE} archive --prefix="${releasename}/" --format=tar HEAD --output "${releasename}.tar")
  endmacro()
  make_release_tarball("openmd_release" "openmd-${VERSION_MAJOR}.${VERSION_MINOR}")
else()
  SET(GIT_SHA1 Release)
endif()

# Build config header
configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

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
	src/DensityOfStates/DOSOrderParameter.cpp
	src/ForceFields/ForceField.cpp
	src/ForceFields/ForceFieldManager.cpp
	src/JSON/jsoncpp.cpp
	src/JSON/schema.cpp
	src/Moves/Move.cpp
	src/Observers/DLMFileObserver.cpp
#	src/Observers/ConsoleObserver.cpp
	src/Observers/JSONObserver.cpp
	src/Observers/XYZObserver.cpp
	src/Particles/Particle.cpp
	src/Simulation/SimObservable.cpp
	src/Simulation/SimObserver.cpp
	src/Simulation/DOSSimulation.cpp
	src/Simulation/Simulation.cpp
	src/Simulation/StandardSimulation.cpp
	src/Utils/Histogram.cpp
	src/Worlds/World.cpp
	src/Validator/RequirementLoader.cpp
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -pthread -std=c++11")
set_target_properties(libsaphron saphron PROPERTIES COMPILE_FLAGS "-Wall -Wextra")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")

if(SAPHRON_OPENMP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fopenmp")
	message(STATUS "*** OpenMP Parallelization enabled!")
	message(STATUS "")
endif()


# Add testing 
enable_testing()
set(TEST_DEPS 
	${SAPH_LIBS} 
	${GTEST_LIBRARY_PATH} 
	${GTEST_LIBRARY_MAIN_PATH}
	libsaphron)

add_executable(DebyeHuckelChargeTests test/DebyeHuckelChargeTests.cpp)
target_link_libraries(DebyeHuckelChargeTests ${TEST_DEPS})
target_include_directories(DebyeHuckelChargeTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DebyeHuckelChargeTests googletest) 
add_test(DebyeHuckelChargeTests DebyeHuckelChargeTests)

add_executable(DeleteParticleMoveTests test/DeleteParticleMoveTests.cpp)
target_link_libraries(DeleteParticleMoveTests ${TEST_DEPS})
target_include_directories(DeleteParticleMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DeleteParticleMoveTests googletest) 
add_test(DeleteParticleMoveTests DeleteParticleMoveTests)

add_executable(DirectorRotateMoveTests test/DirectorRotateMoveTests.cpp)
target_link_libraries(DirectorRotateMoveTests ${TEST_DEPS})
target_include_directories(DirectorRotateMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DirectorRotateMoveTests googletest) 
add_test(DirectorRotateMoveTests DirectorRotateMoveTests)

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

add_executable(DSFPointChargeFFTests test/DSFPointChargeFFTests.cpp)
target_link_libraries(DSFPointChargeFFTests ${TEST_DEPS})
target_include_directories(DSFPointChargeFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(DSFPointChargeFFTests googletest) 
add_test(DSFPointChargeFFTests DSFPointChargeFFTests)

add_executable(ElasticCoeffOPTests test/ElasticCoeffOPTests.cpp)
target_link_libraries(ElasticCoeffOPTests ${TEST_DEPS})
target_include_directories(ElasticCoeffOPTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ElasticCoeffOPTests googletest) 
add_test(ElasticCoeffOPTests ElasticCoeffOPTests)

#add_executable(FENEFFTests test/FENEFFTests.cpp)
#target_link_libraries(FENEFFTests ${TEST_DEPS})
#target_include_directories(FENEFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
#add_dependencies(FENEFFTests googletest) 
#add_test(FENEFFTests FENEFFTests)

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

add_executable(GibbsNVTEnsembleTests test/GibbsNVTEnsembleTests.cpp)
target_link_libraries(GibbsNVTEnsembleTests ${TEST_DEPS})
target_include_directories(GibbsNVTEnsembleTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(GibbsNVTEnsembleTests googletest) 
add_test(GibbsNVTEnsembleTests GibbsNVTEnsembleTests)

add_executable(GrandCanonicalEnsembleTests test/GrandCanonicalEnsembleTests.cpp)
target_link_libraries(GrandCanonicalEnsembleTests ${TEST_DEPS})
target_include_directories(GrandCanonicalEnsembleTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(GrandCanonicalEnsembleTests googletest) 
add_test(GrandCanonicalEnsembleTests GrandCanonicalEnsembleTests)

add_executable(HistogramTests test/HistogramTests.cpp)
target_link_libraries(HistogramTests ${TEST_DEPS})
target_include_directories(HistogramTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(HistogramTests googletest) 
add_test(HistogramTests HistogramTests)

add_executable(InsertParticleMoveTests test/InsertParticleMoveTests.cpp)
target_link_libraries(InsertParticleMoveTests ${TEST_DEPS})
target_include_directories(InsertParticleMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(InsertParticleMoveTests googletest) 
add_test(InsertParticleMoveTests InsertParticleMoveTests)

add_executable(LebwohlLasherFFTests test/LebwohlLasherFFTests.cpp)
target_link_libraries(LebwohlLasherFFTests ${TEST_DEPS})
target_include_directories(LebwohlLasherFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(LebwohlLasherFFTests googletest) 
add_test(LebwohlLasherFFTests LebwohlLasherFFTests)

add_executable(LennardJonesFFTests test/LennardJonesFFTests.cpp)
target_link_libraries(LennardJonesFFTests ${TEST_DEPS})
target_include_directories(LennardJonesFFTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(LennardJonesFFTests googletest) 
add_test(LennardJonesFFTests LennardJonesFFTests)

add_executable(MoleculeTests test/MoleculeTests.cpp)
target_link_libraries(MoleculeTests ${TEST_DEPS})
target_include_directories(MoleculeTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(MoleculeTests googletest) 
add_test(MoleculeTests MoleculeTests)

add_executable(MoveManagerTests test/MoveManagerTests.cpp)
target_link_libraries(MoveManagerTests ${TEST_DEPS})
target_include_directories(MoveManagerTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(MoveManagerTests googletest) 
add_test(MoveManagerTests MoveManagerTests)

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

add_executable(ParticleSwapMoveTests test/ParticleSwapMoveTests.cpp)
target_link_libraries(ParticleSwapMoveTests ${TEST_DEPS})
target_include_directories(ParticleSwapMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ParticleSwapMoveTests googletest) 
add_test(ParticleSwapMoveTests ParticleSwapMoveTests)

add_executable(RandomIdentityMoveTests test/RandomIdentityMoveTests.cpp)
target_link_libraries(RandomIdentityMoveTests ${TEST_DEPS})
target_include_directories(RandomIdentityMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(RandomIdentityMoveTests googletest) 
add_test(RandomIdentityMoveTests RandomIdentityMoveTests)

add_executable(RandTests test/RandTests.cpp)
target_link_libraries(RandTests ${TEST_DEPS})
target_include_directories(RandTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(RandTests googletest) 
add_test(RandTests RandTests)

add_executable(RotateMoveTests test/RotateMoveTests.cpp)
target_link_libraries(RotateMoveTests ${TEST_DEPS})
target_include_directories(RotateMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(RotateMoveTests googletest) 
add_test(RotateMoveTests RotateMoveTests)

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

add_executable(ThreeInteractions test/ThreeInteractions.cpp)
target_link_libraries(ThreeInteractions ${TEST_DEPS})
target_include_directories(ThreeInteractions PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ThreeInteractions googletest) 
add_test(ThreeInteractions ThreeInteractions)

add_executable(TranslateMoveTests test/TranslateMoveTests.cpp)
target_link_libraries(TranslateMoveTests ${TEST_DEPS})
target_include_directories(TranslateMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(TranslateMoveTests googletest) 
add_test(TranslateMoveTests TranslateMoveTests)

add_executable(VolumeSwapMoveTests test/VolumeSwapMoveTests.cpp)
target_link_libraries(VolumeSwapMoveTests ${TEST_DEPS})
target_include_directories(VolumeSwapMoveTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(VolumeSwapMoveTests googletest) 
add_test(VolumeSwapMoveTests VolumeSwapMoveTests)

add_executable(ValidatorTests test/ValidatorTests.cpp)
target_link_libraries(ValidatorTests ${TEST_DEPS})
target_include_directories(ValidatorTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(ValidatorTests googletest) 
add_test(ValidatorTests ValidatorTests)

add_executable(WorldManagerTests test/WorldManagerTests.cpp)
target_link_libraries(WorldManagerTests ${TEST_DEPS})
target_include_directories(WorldManagerTests PRIVATE "${GTEST_INCLUDE_DIR}")
add_dependencies(WorldManagerTests googletest) 
add_test(WorldManagerTests WorldManagerTests)

# end add testing 

install(TARGETS saphron libsaphron 
	RUNTIME DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
	ARCHIVE DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

message(STATUS "CMAKE_CXX_FLAGS           = ${CMAKE_CXX_FLAGS}"          )
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "CMAKE_REQUIRED_INCLUDES   = ${CMAKE_REQUIRED_INCLUDES}"  )
