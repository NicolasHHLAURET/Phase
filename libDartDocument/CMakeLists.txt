add_library(dart_document STATIC
	src/DartException.cpp
	src/DartExceptionDocumentIO.cpp
	src/DartExceptionProperty.cpp
	src/DartSAParameters.cpp
	src/DartXML.cpp
)

target_include_directories(dart_document PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dart_document PUBLIC dart_env pugixml::pugixml)
