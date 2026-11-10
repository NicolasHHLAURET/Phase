SET(LIB DartBase)

SET(SRC
	Exit_if.cpp
	Lib_fileManager.cpp
	Lib_malloc.cpp
	Lib_math.cpp
	Lib_util.cpp
	Lib_physic.cpp
	Vector_tools.cpp
)

INCLUDE_DIRECTORIES(../../libBoost)
ADD_LIBRARY(${LIB} ${SRC})

TARGET_LINK_LIBRARIES(${LIB}
	soci_core_lib
	soci_sqlite3_lib
	sqlite_lib
	${LIBSQL}
)
