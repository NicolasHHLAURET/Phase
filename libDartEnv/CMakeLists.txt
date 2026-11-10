add_library(dart_env STATIC
	src/DartEnv.cpp
	src/DartEnvParamPropertiesToUpdate.cpp
)

target_include_directories(dart_env PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dart_env PUBLIC dart_base Boost::filesystem Boost::lexical_cast)
