SET(LIB DartDocument)

SET(SRC
	DartException.cpp
	DartExceptionDocumentIO.cpp
	DartExceptionProperty.cpp
	DartSAParameters.cpp
	DartXML.cpp
)

IF(MINGW)
	INCLUDE_DIRECTORIES(../../build_all/include)
ENDIF(MINGW)
ADD_LIBRARY(${LIB} ${SRC})

TARGET_LINK_LIBRARIES(${LIB}
	libDartEnv.a
	boost_system
	boost_filesystem
	TinyXPath
)
