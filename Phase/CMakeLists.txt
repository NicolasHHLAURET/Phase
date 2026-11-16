add_executable(phase
	src/Phase.cpp
	src/PhaseDAO.cpp
	src/PhaseDTOs.cpp
	src/PhaseProperty.cpp
	src/PhaseSurfaces.cpp
	src/PhaseThermal.cpp
	src/PhaseUtils.cpp
	src/PhaseVegetation.cpp
	src/containers/NearestDirections.cpp
	src/containers/air/AirLaw.cpp
	src/containers/air/AirPhaseFunction.cpp
	src/database/atmosphereModel/AtmosphericDatabaseStructure.cpp
	src/fluorescence/FluorescenceMatrices.cpp
	src/foliar/FoliarComputation.cpp
	src/foliar/FoliarModel.cpp
	src/foliar/LADComputation.cpp
	src/inputs/PhaseIODirections.cpp
	src/inputs/PhaseIOEmissionMaterial.cpp
	src/inputs/PhaseIOOpticalMaterial.cpp
	src/inputs/PhaseIOSpectralBands.cpp
	src/materials/Hapke1981MaterialWriter.cpp
	src/materials/Hapke2012MaterialWriter.cpp
	src/materials/LambertianMaterialWriter.cpp
	src/materials/RPVMaterialWriter.cpp
	src/materials/SurfaceMaterialProcessor.cpp
	src/opticalfactors/OpticalFactorsTextureWriter.cpp
)

target_include_directories(phase PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(phase PRIVATE
	dart_base
	dart_geometry
	dart_env
	dart_document
	dart_core
	Boost::boost
)

if(DART_WITH_NETCDF)
	target_compile_definitions(phase PRIVATE DART_WITH_NETCDF)
endif()
