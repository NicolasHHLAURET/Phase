add_library(dart_geometry STATIC
	src/DirectionLayer.cpp
	src/DirectionRoot.cpp
	src/IcoSphere.cpp
	src/Point3.cpp
	src/Triplet.cpp
	src/UtilitaireFigure.cpp
	src/Vecteur.cpp
)

target_include_directories(dart_geometry PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dart_geometry PUBLIC dart_base)
