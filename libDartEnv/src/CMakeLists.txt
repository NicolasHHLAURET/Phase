SET(LIB DartEnv)
INCLUDE_DIRECTORIES(../../librairieDart)
INCLUDE_DIRECTORIES(../../libBoost)

SET(SRC
	DartEnv.cpp
	DartEnvParamPropertiesToUpdate.cpp
)

ADD_LIBRARY(${LIB} ${SRC})
TARGET_LINK_LIBRARIES(${LIB}
	libDartBase.a
	boost_system
	boost_filesystem
	#libLog.a
)

#########################
#### TESTS UNITAIRES ####
#########################

#ENABLE_TESTING()

#### ---- DartEnv -----
#SET(EXE TestsLibDartEnv_DartEnv)
#SET(SRC
#	DartEnv.cpp
#	DartEnvParamPropertiesToUpdate.cpp
#	testsUnitaires/TestDartEnv.cpp
#)
#ADD_EXECUTABLE(${EXE} ${SRC})
#TARGET_LINK_LIBRARIES(${EXE}
#	${LIBBOOSTUTF}
#	#libDartEnv.a
#	libDartBase.a
#)
#INSTALL(TARGETS ${EXE} DESTINATION ${DARTDIS})
#ADD_TEST(libDartEnv_DartEnv ${DARTDIS}/TestsLibDartEnv_DartEnv.exe)
