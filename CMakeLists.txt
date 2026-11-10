cmake_minimum_required(VERSION 3.16)

project(DARTPhase
	VERSION 0.1.0
	DESCRIPTION "DART phase-function computation module"
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Prefix containing the third-party dependencies (Boost headers, pugixml,
# yaml-cpp, SQLite, SOCI). Override with -DDART_DEPS_PREFIX=/path/to/prefix.
set(DART_DEPS_PREFIX "" CACHE PATH "Installation prefix of the third-party dependencies")
if(NOT DART_DEPS_PREFIX)
	message(FATAL_ERROR "DART_DEPS_PREFIX must point to the third-party dependency prefix")
endif()
list(APPEND CMAKE_PREFIX_PATH "${DART_DEPS_PREFIX}")

# Optional feature toggles
option(DART_WITH_NETCDF "Enable NetCDF/HDF5-based atmosphere and phase-function files" OFF)

find_package(Boost 1.70 REQUIRED)
find_package(pugixml REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SOCI REQUIRED COMPONENTS SQLite3)

foreach(dart_module libDartBase libDartGeometry libDartEnv libDartDocument librairieDart Phase)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dart_module}/CMakeLists.txt)
		add_subdirectory(${dart_module})
	else()
		message(STATUS "Skipping module ${dart_module}: no CMakeLists.txt")
	endif()
endforeach()
