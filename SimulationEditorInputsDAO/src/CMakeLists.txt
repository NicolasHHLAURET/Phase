SET(LIB SimulationEditorInputsDAO)

SET(SRC
	yaml/YaMLContainer.cpp
	yaml/YaMLReader.cpp
		
	# Objects
	model/object/genericObject/DoubletDouble.cpp
	model/object/genericObject/DoubletInteger.cpp
	model/object/genericObject/TripletDouble.cpp
	model/object/geometry/AngleDirection.cpp
	model/object/geometry/EarthCoordinate.cpp
	model/object/geometry/SubScene.cpp
	model/object/database/illuminationDatabase/AtmosphericIlluminationDatabase.cpp
	model/object/database/atmosphereDatabase/AerosolModelDatabase.cpp
	model/object/database/atmosphereDatabase/GasModelDatabase.cpp
	model/object/database/opticalDatabase/DartModelDatabase.cpp
		
	# DartAirborneSensors.yml
	model/airborneSensor/pushbroomDefinition/IPusbroomScannerDefinition.cpp
	model/airborneSensor/pushbroomDefinition/PushbroomScannerWithAngleDistributionPath.cpp
	model/airborneSensor/pushbroomDefinition/PushbroomScannerWithLinearPath.cpp
	
	model/airborneSensor/insideSensorDefinition/IInsideSensorViewDirection.cpp
	model/airborneSensor/insideSensorDefinition/InsideSensorViewDefinitionAngle.cpp
	model/airborneSensor/insideSensorDefinition/InsideSensorViewDefinitionAxis.cpp
	
	model/airborneSensor/pinholeDefinition/IPinholeCameraDefinition.cpp
	model/airborneSensor/pinholeDefinition/PinholeCameraRotation.cpp
	model/airborneSensor/pinholeDefinition/noHemispherical/IPinholeCameraAngleDefinition.cpp
	model/airborneSensor/pinholeDefinition/noHemispherical/angleDefinition/PinholeCameraAngleOfView.cpp
	model/airborneSensor/pinholeDefinition/noHemispherical/angleDefinition/PinholeCameraFOVSize.cpp
	
	model/airborneSensor/pinholeDefinition/noHemispherical/PinholeCameraNumberPixelsInFOV.cpp
	model/airborneSensor/pinholeDefinition/noHemispherical/PinholeCameraFOV.cpp
	model/airborneSensor/pinholeDefinition/noHemispherical/PinholeCameraDefinition.cpp
	model/airborneSensor/pinholeDefinition/hemispherical/HemisphericalFOV.cpp
	model/airborneSensor/pinholeDefinition/hemispherical/HemisphericalCameraDefinition.cpp
	
	model/airborneSensor/ISensor.cpp
	model/airborneSensor/InsideSensor.cpp
	model/airborneSensor/PinholeCamera.cpp
	model/airborneSensor/PushbroomScanner.cpp
	
	model/DartAirborneSensors.cpp
		
	# DartAtmosphere.yml
	model/atmosphere/IAtmosphereModel.cpp
	model/atmosphere/DatabaseAtmosphereModel.cpp
	model/atmosphere/UserDefinedAtmosphereModel.cpp
	model/atmosphere/AerosolModel.cpp
	
	model/atmosphere/atmosphereGeometry/IDiscretizationMode.cpp
	model/atmosphere/atmosphereGeometry/ManualDiscretization.cpp
	model/atmosphere/atmosphereGeometry/FastDiscretization.cpp
	model/atmosphere/atmosphereGeometry/AutomaticDiscretization.cpp	
	model/atmosphere/atmosphereGeometry/AtmosphereGeometry.cpp	
	
	model/atmosphere/atmosphereFluxTracking/ICalculationMode.cpp	
	model/atmosphere/atmosphereFluxTracking/CalculationPreComputed.cpp	
	model/atmosphere/atmosphereFluxTracking/CalculationOnTheFly.cpp	
	model/atmosphere/atmosphereFluxTracking/AtmosphereFluxTracking.cpp	
	
	model/DartAtmosphere.cpp
	
	# DartDirections.yml
	model/direction/viewDirections/directionShape/ISquareWidthDefinition.cpp
	model/direction/viewDirections/directionShape/SquareWidthAzimuth.cpp
	model/direction/viewDirections/directionShape/SquareWidthSolidAngle.cpp
	model/direction/viewDirections/directionShape/SquareWidthZenith.cpp
	model/direction/viewDirections/directionShape/IDirectionShape.cpp
	model/direction/viewDirections/directionShape/SquareShape.cpp
	model/direction/viewDirections/directionShape/RectangleShape.cpp
	
	model/direction/viewDirections/hotspot/HotspotRegion.cpp
	model/direction/viewDirections/hotspot/HotspotDirectionsSet.cpp
	model/direction/viewDirections/hotspot/HotspotDirections.cpp
	
	model/direction/viewDirections/IDirectionDescription.cpp
	model/direction/viewDirections/AngleDescription.cpp
	model/direction/viewDirections/CoordinateDescription.cpp
	model/direction/viewDirections/IAddedViewDirections.cpp
	model/direction/viewDirections/OversampledPlane.cpp
	model/direction/viewDirections/Region.cpp
	model/direction/viewDirections/SingleDirection.cpp
	
	model/direction/sunDirection/sunPenumbra/ISunPenumbra.cpp
	model/direction/sunDirection/sunPenumbra/IcospherePenumbra.cpp
	model/direction/sunDirection/sunPenumbra/MonteCarloPenumbra.cpp
	model/direction/sunDirection/sunPenumbra/NoSunPenumbra.cpp
	
	model/direction/sunDirection/LocalTime.cpp
	model/direction/sunDirection/ISunAngleDefinition.cpp
	model/direction/sunDirection/DateSunAngle.cpp
	model/direction/sunDirection/DownwardSunAngle.cpp
	model/direction/sunDirection/ViewingAngle.cpp
	model/direction/sunDirection/SunDirection.cpp
	
	model/DartDirections.cpp
	
	# DartProducts.yaml
	model/products/earthSceneProducts/LAIProducts.cpp
	model/products/earthSceneProducts/TriangleGroundCoverProducts.cpp
	model/products/earthSceneProducts/TrianglePerCellAreaProducts.cpp
	model/products/AtmosphericProducts.cpp
	model/products/EarthSceneProducts.cpp
	model/products/FluxTrackingProducts.cpp
	model/products/LidarImageProducts.cpp
	model/products/LidarProducts.cpp
	model/products/RadiativeBudgetProducts.cpp
	model/DartProducts.cpp
	
	# DartEarthSceneProperties.yml
	model/modelisation/objectColor/DartColor.cpp
	model/modelisation/objectColor/DartsRGB.cpp
	
	model/modelisation/topography/TopographyFile.cpp
	model/modelisation/waterSurface/WaterSurfacesProperties.cpp
	model/modelisation/EarthSceneProperties.cpp
	model/DartEarthSceneProperties.cpp
	
	# DartExecution.yml
	model/execution/externalScripts/scriptLocation/IScriptLocation.cpp
	model/execution/externalScripts/scriptLocation/AfterDartProcess.cpp
	model/execution/externalScripts/scriptLocation/BeforeDartProcess.cpp
	model/execution/externalScripts/scriptLocation/BetweenDartModules.cpp
	model/execution/externalScripts/IExternalScript.cpp
	model/execution/externalScripts/PythonExternalScript.cpp
	model/execution/ExternalScripts.cpp
	model/execution/ExecutionParameters.cpp
	model/DartExecution.cpp
	
	# DartOpticalProperties.yml	
	model/phase/phaseFunction/opticalProperty/prospect/FluorescenceProperties.cpp
	model/phase/phaseFunction/opticalProperty/prospect/FluorescenceFunction.cpp
	model/phase/phaseFunction/opticalProperty/prospect/ProspectFunction.cpp
	
	model/phase/phaseFunction/vegetationFunction/IDirectionalWeightOfGFunction.cpp
	model/phase/phaseFunction/vegetationFunction/DirectionalWeightOfGFunction.cpp
	
	model/phase/phaseFunction/leafAngleDistribution/ILeafAngleDistribution.cpp
	model/phase/phaseFunction/leafAngleDistribution/BoundedUniform.cpp
	model/phase/phaseFunction/leafAngleDistribution/DeWitFunction.cpp
	model/phase/phaseFunction/leafAngleDistribution/Ellipsoidal.cpp
	model/phase/phaseFunction/leafAngleDistribution/Elliptical.cpp
	model/phase/phaseFunction/leafAngleDistribution/Erectophile.cpp
	model/phase/phaseFunction/leafAngleDistribution/Extremophile.cpp
	model/phase/phaseFunction/leafAngleDistribution/Horizontal.cpp
	model/phase/phaseFunction/leafAngleDistribution/Plagiophile.cpp
	model/phase/phaseFunction/leafAngleDistribution/Planophile.cpp
	model/phase/phaseFunction/leafAngleDistribution/Spherical.cpp
	model/phase/phaseFunction/leafAngleDistribution/Uniform.cpp
	model/phase/phaseFunction/leafAngleDistribution/UserDefined.cpp
	model/phase/phaseFunction/leafAngleDistribution/Vertical.cpp	
	
	model/phase/phaseFunction/multiplicativeFactor/property/IMultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/IMultiplicativeFactorQuantifier.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/MultiplicativeFactorForBand.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/MultiplicativeFactorGlobal.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/VegetationMultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/LambertianMultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/RpvMultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/property/HapkeMultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/MultiplicativeFactor.cpp
	model/phase/phaseFunction/multiplicativeFactor/MultiplicativeFactorForAllBands.cpp
	model/phase/phaseFunction/multiplicativeFactor/MultiplicativeFactorPerBand.cpp
	
	model/phase/thermalFunction/IThermalProperty.cpp
	model/phase/thermalFunction/ThermalProperty.cpp
	
	model/phase/phaseFunction/specular/SpecularParameters.cpp
	
	model/phase/phaseFunction/opticalProperty/IOpticalProperty.cpp
	model/phase/phaseFunction/opticalProperty/IPhaseProperty.cpp
	model/phase/phaseFunction/opticalProperty/HapkeProperty.cpp
	model/phase/phaseFunction/opticalProperty/LambertianProperty.cpp
	model/phase/phaseFunction/opticalProperty/RPVProperty.cpp
	model/phase/phaseFunction/opticalProperty/FluidProperty.cpp
	model/phase/phaseFunction/opticalProperty/VegetationProperty.cpp
	model/phase/phaseFunction/opticalProperty/PhaseExtern.cpp
	
	model/DartOpticalProperties.cpp
	
	# DartSceneObjects.yaml
	model/modelisation/plots/extraPlots/ExtraPlots.cpp
	
	model/modelisation/IDartObject.cpp
	
	model/modelisation/ground/IDartGroundArea.cpp
	model/modelisation/ground/EllipseGroundArea.cpp
	model/modelisation/ground/PolygonGroundArea.cpp
	model/modelisation/ground/RectangleGroundArea.cpp
	
	model/modelisation/urbanStructures/roof/IHouseRoof.cpp
	model/modelisation/urbanStructures/roof/ClassicRoof.cpp
	model/modelisation/urbanStructures/roof/ComplexRoof.cpp
	model/modelisation/urbanStructures/roof/NoRoof.cpp
	model/modelisation/urbanStructures/roof/PlateRoof.cpp
	
	model/modelisation/urbanStructures/object/Vertex.cpp
	model/modelisation/urbanStructures/object/Face.cpp
	model/modelisation/urbanStructures/IBuildingObject.cpp
	model/modelisation/urbanStructures/base/WallBase.cpp
	model/modelisation/urbanStructures/SmallWall.cpp
	model/modelisation/urbanStructures/House.cpp
	
	model/modelisation/object3d/physicalProperties/laiComputing/ILAIComputing.cpp
	model/modelisation/object3d/physicalProperties/laiComputing/LAICalculated.cpp
	model/modelisation/object3d/physicalProperties/laiComputing/LAIRelativeToTreeProjection.cpp
	model/modelisation/object3d/physicalProperties/laiComputing/LAITotalLeafAreaProjection.cpp
	
	model/modelisation/object3d/physicalProperties/ILADConversion.cpp
	model/modelisation/object3d/physicalProperties/CalculatedLAD.cpp
	model/modelisation/object3d/physicalProperties/NoConversionLAD.cpp
	model/modelisation/object3d/physicalProperties/PredefinedLAD.cpp
	
	model/modelisation/object3d/Group.cpp
	model/modelisation/object3d/GroupLeaf.cpp
	model/modelisation/object3d/GroupTwig.cpp
	
	model/modelisation/object3d/IObject3DNumber.cpp
	model/modelisation/object3d/Object3DSingle.cpp
	model/modelisation/object3d/Object3DField.cpp
	
	model/modelisation/object3d/Object3DGeometry.cpp
	model/modelisation/object3d/Object3D.cpp
	
	model/modelisation/plots/plotForm/IPlotForm.cpp
	model/modelisation/plots/plotForm/Plot4Corners.cpp
	model/modelisation/plots/plotForm/PlotRectangle.cpp	
		
	model/modelisation/plots/plotType/IPlotType.cpp
	model/modelisation/plots/plotType/FluidPlotType.cpp
	model/modelisation/plots/plotType/GroundAndVegetationPlotType.cpp	
	model/modelisation/plots/plotType/GroundPlotType.cpp	
	model/modelisation/plots/plotType/VegetationPlotType.cpp	
	model/modelisation/plots/plotType/WaterPlotType.cpp	
	
	model/modelisation/plots/Plot.cpp
	
	model/modelisation/multipleTrees/position/randomPosition/IRandomSpatialDistribution.cpp
	model/modelisation/multipleTrees/position/randomPosition/GridOnEllipticalSubZone.cpp
	model/modelisation/multipleTrees/position/randomPosition/GridOnQuadrilateralSubZone.cpp
	model/modelisation/multipleTrees/position/randomPosition/GridOnTotalScene.cpp
	
	model/modelisation/multipleTrees/position/ITreesPositions.cpp
	model/modelisation/multipleTrees/position/ExactPosition.cpp
	model/modelisation/multipleTrees/position/RandomPosition.cpp
	
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/ICrownShapeRandomDimension.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/RandomComposedEllipsoidCrown.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/RandomConiqueComposedCrown.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/RandomEllipsoideCrown.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/RandomTrapezoidCrown.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/crownShape/RandomTruncatedConeCrown.cpp
	
	model/modelisation/multipleTrees/specie/specieDimensions/ISpecieDimensions.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/ExactSpecieDimensions.cpp
	model/modelisation/multipleTrees/specie/specieDimensions/RandomSpecieDimensions.cpp
	
	model/modelisation/multipleTrees/specie/crownLevel/crownLeafDistribution/ICrownLeafDistribution.cpp
	model/modelisation/multipleTrees/specie/crownLevel/crownLeafDistribution/Holes.cpp
	model/modelisation/multipleTrees/specie/crownLevel/crownLeafDistribution/Leaves.cpp
	model/modelisation/multipleTrees/specie/crownLevel/crownLeafDistribution/LeavesAndHoles.cpp
	model/modelisation/multipleTrees/specie/crownLevel/crownLeafDistribution/LeavesAndHolesUniform.cpp
	
	model/modelisation/multipleTrees/specie/crownLevel/CrownLevel.cpp
	model/modelisation/multipleTrees/specie/BranchAndTwig.cpp
	model/modelisation/multipleTrees/specie/Specie.cpp
	model/modelisation/multipleTrees/MultipleTrees.cpp
	
	model/DartSceneObjects.cpp
	
	# DartRadiation.yaml
	model/radiation/radiativeTransfer/temperature/ISceneTemperature.cpp
	model/radiation/radiativeTransfer/temperature/UserDefinedSceneTemperature.cpp
	
	model/radiation/radiativeTransfer/illumination/IAtmosphericIlumination.cpp
	model/radiation/radiativeTransfer/illumination/BandIrradiance.cpp
	model/radiation/radiativeTransfer/illumination/BandRadiance.cpp
	model/radiation/radiativeTransfer/illumination/SpectralIrradiance.cpp
	model/radiation/radiativeTransfer/illumination/SpectralRadiation.cpp
	
	model/radiation/radiativeTransfer/fluxTracking/FluxTrackingAdvancedParameters.cpp
	model/radiation/radiativeTransfer/fluxTracking/FluxTrackingParameters.cpp
	
	model/radiation/radiativeTransfer/monteCarlo/MonteCarloAdvancedParameters.cpp
	model/radiation/radiativeTransfer/monteCarlo/MonteCarloParameters.cpp
	
	model/radiation/radiativeTransfer/lidar/geometry/footPrint/IFootPrint.cpp
	model/radiation/radiativeTransfer/lidar/geometry/footPrint/HalfAngle.cpp
	model/radiation/radiativeTransfer/lidar/geometry/footPrint/Radii.cpp
	
	model/radiation/radiativeTransfer/lidar/geometry/waveformStorage/IWaveformStorage.cpp
	model/radiation/radiativeTransfer/lidar/geometry/waveformStorage/WaveformFromAbsoluteDistance.cpp
	model/radiation/radiativeTransfer/lidar/geometry/waveformStorage/WaveformFromMinimumAltitude.cpp
	
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ALS/controlPoint/IControlPoint.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ALS/controlPoint/ControlPointGroundGCP.cpp
	
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ILaserScanningMode.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ALS/ALSMultiPulseFile.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ALS/ALSMultiPulseUserDefined.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/ALS/ALSSinglePulse.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/TLS/TLSMultiPulseFile.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/TLS/TLSMultiPulseUserDefined.cpp
	model/radiation/radiativeTransfer/lidar/geometry/laserScanningMode/TLS/TLSSinglePulse.cpp
	
	model/radiation/radiativeTransfer/lidar/LidarParameters.cpp
	model/radiation/radiativeTransfer/lidar/LidarSolarNoise.cpp
	model/radiation/radiativeTransfer/lidar/geometry/LidarGeometry.cpp
	
	model/radiation/radiativeTransfer/IRadiativeTransferMethod.cpp
	model/radiation/radiativeTransfer/FluxTracking.cpp
	model/radiation/radiativeTransfer/Lidar.cpp
	model/radiation/radiativeTransfer/MonteCarlo.cpp
	
	model/DartRadiation.cpp

	# DartSpectralBands.yml
	model/radiation/spectralBand/spectralDomain/ISpectralDomain.cpp
	model/radiation/spectralBand/spectralDomain/ModeR.cpp
	model/radiation/spectralBand/spectralDomain/ModeRT.cpp
	model/radiation/spectralBand/spectralDomain/ModeT.cpp
	
	model/radiation/spectralBand/SpectralBand.cpp
	model/radiation/spectralBand/SpectralRange.cpp
		
	model/DartSpectralBands.cpp
	
	# The DAO
	DartInputsDAO.cpp
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
	sqlite_lib
	soci_core_lib
	soci_sqlite3_lib
	libDartEnv.a
	libSimulationEditorInputsDAO.a
	${LIBSQL}
)
