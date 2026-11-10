SET(EXE phase)

SET(SRC
	../../librairieDart/src/mapping/PhaseKeyMappingGenerator.cpp
	#containers/DirectionRoot.cpp
	containers/NearestDirections.cpp
	containers/YaMLContainer.cpp
	database/atmosphereModel/AtmosphericDatabaseStructure.cpp
	foliar/FoliarModel.cpp
	foliar/LADComputation.cpp
	foliar/FoliarComputation.cpp
	inputs/PhaseIOEmissionMaterial.cpp
	fluorescence/FluorescenceMatrices.cpp
	materials/LambertianMaterialWriter.cpp
	materials/Hapke1981MaterialWriter.cpp
	materials/SurfaceMaterialProcessor.cpp
	opticalfactors/OpticalFactorsTextureWriter.cpp
	PhaseProperty.cpp
#	PhaseDTOs.cpp
	PhaseDAO.cpp
	PhaseThermal.cpp
	Phase.cpp
	PhaseUtils.cpp
	containers/air/AirLaw.cpp
	containers/air/AirPhaseFunction.cpp
)

IF(MINGW)
	INCLUDE_DIRECTORIES(../../build_all/include)
ENDIF(MINGW)
INCLUDE_DIRECTORIES(../../libSQLite)
INCLUDE_DIRECTORIES(../../libSOCI/src/core)
INCLUDE_DIRECTORIES(../../libSOCI/src/backends/sqlite3)
INCLUDE_DIRECTORIES(../../YamlCpp)
INCLUDE_DIRECTORIES(../../libDependencies)
ADD_EXECUTABLE(${EXE} ${SRC})

ADD_DEPENDENCIES(${EXE} memory_allocator)
ADD_DEPENDENCIES(${EXE} Boost.Test)
ADD_DEPENDENCIES(${EXE} soci_sqlite3)
ADD_DEPENDENCIES(${EXE} YamlCpp.make)
ADD_DEPENDENCIES(${EXE} Dart)
ADD_DEPENDENCIES(${EXE} DartEnv)
ADD_DEPENDENCIES(${EXE} DartBase)
ADD_DEPENDENCIES(${EXE} DartDocument)
ADD_DEPENDENCIES(${EXE} DartGeometry)
ADD_DEPENDENCIES(${EXE} SimulationEditorInputsDAO)

## Add rt librairie for UNIX* version
## This librairie seems to not be be mandatory for recent UNIX64 (like Xubuntu 14.04).
## No need appear for Windows*
## UNIX is for local compilation
TARGET_LINK_LIBRARIES(${EXE}
	glibc_compat
	${TCMALLOC}
	${PSAPI_LIB}
	libDart.a
	soci_core_lib
	soci_sqlite3_lib
	sqlite_lib
	libyaml-cpp.a
	boost_thread
	${LIBRT}
	libDartEnv.a
	boost_system
	boost_filesystem
	libDartBase.a
	libDartDocument.a
	libDartGeometry.a
	libTinyXPath.a
	libSimulationEditorInputsDAO.a
	${LIBSQL}
	libnetcdf_c++4.a
	libnetcdf.a
	libhdf5_hl.a
	libhdf5.a
	${LIBOPENMPI}
	libz.a
)

INSTALL(TARGETS ${EXE} DESTINATION ${DARTDIS})


