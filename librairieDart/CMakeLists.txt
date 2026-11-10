add_library(dart_core STATIC
	src/AccesFichiers.cpp
	src/FichierProperties.cpp
	src/GestOption.cpp
	src/LibDartException.cpp
	src/Option.cpp
	src/SynchronisedQueue.cpp
	src/TypeOption.cpp
	src/mapping/PhaseKeyMappingGenerator.cpp
	src/databases/DartDatabaseManager.cpp
	src/databases/optical/LireDatabase2D.cpp
	src/databases/optical/BaseDeDonneesReflectance.cpp
	src/databases/crossSection/CrossSectionDatabaseDAO.cpp
)

target_include_directories(dart_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dart_core PUBLIC
	dart_base
	dart_env
	Boost::boost
	Boost::thread
	SOCI::Core
	SOCI::SQLite3
	SQLite::SQLite3
)
