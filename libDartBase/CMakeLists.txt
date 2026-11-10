add_library(dart_base STATIC
	src/Exit_if.cpp
	src/Lib_fileManager.cpp
	src/Lib_malloc.cpp
	src/Lib_math.cpp
	src/Lib_physic.cpp
	src/Lib_util.cpp
	src/Vector_tools.cpp
)

target_include_directories(dart_base PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dart_base PUBLIC Boost::boost)
