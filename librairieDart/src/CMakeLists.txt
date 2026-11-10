SET(LIB Dart)

SET(SRC
	GestOption.cpp
	Option.cpp
	TypeOption.cpp
	AccesFichiers.cpp
	databases/lut/BaseDeDonnees.cpp
	databases/inversion/BaseDeDonneesInversion.cpp
	databases/optical/LireDatabase2D.cpp
	databases/optical/BaseDeDonneesReflectance.cpp
	databases/lut/object/ObjectDatabase.cpp
	databases/waterVapor/WaterVaporDatabaseDAO.cpp
	databases/crossSection/CrossSectionDatabaseDAO.cpp
	FichierProperties.cpp
	LibDartException.cpp
	MakInversionParser.cpp
	SynchronisedQueue.cpp
	libIlwis/ReadIlwis.cpp
	libIlwis/WriteIlwis.cpp
	write/JobWriter.cpp
	mapping/PhaseKeyMappingGenerator.cpp
	netcdf/object/DartImageNcGroup.cpp
	netcdf/DartNetcdf.cpp
	netcdf/DartNetcdfFile.cpp
	databases/inversion/cache/CacheInterpolationParameters.cpp
	databases/inversion/cache/object/InterpolationParametersKeys.cpp
	databases/inversion/cache/object/InterpolationParametersValues.cpp
	databases/inversion/cache/CacheInterpolationCoefficients.cpp
	databases/inversion/cache/object/InterpolationCoefficientsKeys.cpp
	databases/inversion/cache/object/InterpolationCoefficientsValues.cpp
	databases/DartDatabaseManager.cpp
	yaml/YaMLContainer.cpp
	yaml/YaMLReader.cpp
)

IF(MINGW)
	INCLUDE_DIRECTORIES(../../build_all/include)
ENDIF(MINGW)
INCLUDE_DIRECTORIES(../../libBoost/boost)
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../../libSQLite)
INCLUDE_DIRECTORIES(../../libSOCI/src/core)
INCLUDE_DIRECTORIES(../../libSOCI/src/backends/sqlite3)
ADD_LIBRARY(${LIB} ${SRC})

TARGET_LINK_LIBRARIES(${LIB}
	glibc_compat
	${TCMALLOC}
	${PSAPI_LIB}
	sqlite_lib
	soci_core_lib
	soci_sqlite3_lib
	libDartEnv.a
	${LIBSQL}
	boost_thread
	boost_filesystem
	boost_system
	libnetcdf_c++4.a
	libnetcdf.a
	libhdf5_hl.a
	libhdf5.a
	${LIBOPENMPI}
	libz.a
)


#ENABLE_TESTING()

#### ---- BaseDeDonnees -----
#SET(EXE TestsLibDart_BaseDeDonnees)
#SET(SRC
#classes/objetDART/ParBande.cpp
#classes/objetDART/DartProprietes.cpp
#	BaseDeDonnees.cpp
#	TestBaseDeDonnees.cpp
#classes/BaseDeDonnees.cpp
#testsUnitaires/TestBaseDeDonnees.cpp
#)
#ADD_EXECUTABLE(${EXE} ${SRC})
#TARGET_LINK_LIBRARIES(${EXE}
#	${LIBBOOSTUTF}
#	soci_core_lib
#	soci_sqlite3_lib
#	sqlite_lib
#	${LIBSQL}
#${CMAKE_THREAD_LIBS_INIT}
#${CMAKE_DL_LIBS}
#libsqlite3.a
#	libDart.a
#libDartBase.a
#)
#INSTALL(TARGETS ${EXE} DESTINATION ${DARTDIS})
#ADD_TEST(Dart_BaseDeDonnees ${DARTDIS}/TestsLibDart_BaseDeDonnees.exe)
