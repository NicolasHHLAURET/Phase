SET(LIB DartGeometry)

SET(SRC
	Point3.cpp
	Triplet.cpp
	Vecteur.cpp
	DirectionRoot.cpp
	DirectionLayer.cpp
	UtilitaireFigure.cpp
	IcoSphere.cpp
	)
	
IF(MINGW)
	INCLUDE_DIRECTORIES(../../build_all/include)
ENDIF(MINGW)
INCLUDE_DIRECTORIES(../../libBoost/boost)

ADD_LIBRARY(${LIB} ${SRC})
TARGET_LINK_LIBRARIES(${LIB}
	${TCMALLOC}
)

