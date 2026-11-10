/*
 * PhaseSurfaces.cpp
 *
 *  Created on: 20 oct. 2026
 *      Author: lauretn
 */

#include "Phase.hpp"

void treatHapke(OpticalPropertyParameters & hapkePropertyParameters,
				unsigned int opIndex,
				const vector<bool> & useSpectralIrradianceForReflectance,
				PhaseKeyMappingGenerator & pkmg,
				ofstream & outCoeffSpecProperties,
				PhaseGeneralParametersDTO & genDTO,
				const vector<DirectionCartesienne> & listDirections) {
	pkmg.setTypeFonctionDePhase(globalReferences::HAPKE_SPEC);
	double albedo = 0;
	double dTauMoy = 0;
	vector<double>w(genDTO.numberOfSpectralBands),c1(genDTO.numberOfSpectralBands),c2(genDTO.numberOfSpectralBands),
			c3(genDTO.numberOfSpectralBands), c4(genDTO.numberOfSpectralBands), h1(genDTO.numberOfSpectralBands),
			h2(genDTO.numberOfSpectralBands), diffuseTransmittance(genDTO.numberOfSpectralBands);

	for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		PropertyHapke hapkeProperty;

		computeOpticalPropertiesHapke(spectralBandIndex, hapkePropertyParameters, listDirections, hapkeProperty);
		computeAlbedodTauMoyHapke(hapkeProperty, albedo, dTauMoy, listDirections);
		testHapkeAlbedo(hapkePropertyParameters.getModelIndex(), hapkeProperty);

		w[spectralBandIndex] = hapkeProperty.hapkeParameterW;
		c1[spectralBandIndex] = hapkeProperty.hapkeParameterC1;
		c2[spectralBandIndex] = hapkeProperty.hapkeParameterC2;
		c3[spectralBandIndex] = hapkeProperty.hapkeParameterC3;
		c4[spectralBandIndex] = hapkeProperty.hapkeParameterC4;
		h1[spectralBandIndex] = hapkeProperty.hapkeParameterH1;
		h2[spectralBandIndex] = hapkeProperty.hapkeParameterH2;
		diffuseTransmittance[spectralBandIndex] = hapkeProperty.diffuseTransmittance;
	}

	std::ostringstream keyRoot;
	keyRoot << "scene.materials." << "Hapke" <<  hapkePropertyParameters.getIndexOrder() /*<< "_" << tmpIndex*/ << ".";
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "type", "Hapke1981");

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "generator", "phase");
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "id", std::to_string(opIndex));
//			genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "temperatureIndex", std::to_string(tmpIndex));
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opIndex", std::to_string(hapkePropertyParameters.getModelIndex()));

	// Reflectance
	std::ostringstream valueW,valueC1,valueC2,valueC3, valueC4, valueH1, valueH2, valueKt, valueThetaMax, emissivity;
	valueW << setprecision(15);
	valueC1 << setprecision(15);
	valueC2 << setprecision(15);
	valueC3 << setprecision(15);
	valueC4 << setprecision(15);
	valueH1 << setprecision(15);
	valueH2 << setprecision(15);
	valueKt << setprecision(15);
	valueThetaMax << setprecision(15);
	emissivity << setprecision(15);
	valueW << w[0]; valueC1<<c1[0]; valueC2<<c2[0]; valueC3<<c3[0]; valueC4<<c4[0]; valueH1<<h1[0]; valueH2<<h2[0]; valueKt<<((diffuseTransmittance[0]<=1e-8) ? 0.0 : diffuseTransmittance[0]/(1 - w[0]));
	emissivity << 1.; // Force emissivity to 1, the real emissivity is computed in dart
	for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
		valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
		valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
		valueH2<< " " <<h2[spectralBandIndex];
		valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
		emissivity << " " << 0.;
	}

	if (hapkePropertyParameters.useDirectionalDirectTransmittance()) {
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.angularStep", std::to_string(hapkePropertyParameters._angularStep));
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "transparency.directionalFile", hapkePropertyParameters._ddtFilePath);
	}

	if (PhaseProperty::getPolarization()) {
		// 3 * numberOfSpectralBands are added to simulate Q, U, V
		// The optical properties are repeated in these bands
		for (unsigned int stokesCompoent = 0; stokesCompoent < 3; ++stokesCompoent) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
				valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
				valueH2<< " " <<h2[spectralBandIndex];
				valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
				emissivity << " " << 0.;
			}
		}
	}

	if (PhaseProperty::getFluorescence()) {
		// 2 * numberOfSpectralBands are added to simulate PSI and PSII
		// The optical properties are repeated in these bands
		for (unsigned int photoSystemIndex = 0; photoSystemIndex < 2; ++photoSystemIndex) {
			for (unsigned int spectralBandIndex = 0; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				valueW << " " << w[spectralBandIndex]; valueC1<< " " <<c1[spectralBandIndex]; valueC2<< " " <<c2[spectralBandIndex];
				valueC3<< " " <<c3[spectralBandIndex]; valueC4<< " " <<c4[spectralBandIndex]; valueH1<< " " <<h1[spectralBandIndex];
				valueH2<< " " <<h2[spectralBandIndex];
				valueKt<< " " << ((diffuseTransmittance[spectralBandIndex]<=1e-8) ? 0.0 : diffuseTransmittance[spectralBandIndex]/(1 - w[spectralBandIndex]));
				emissivity << " " << 0.;
			}
		}
	}

	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "w", valueW.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c1", valueC1.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c2", valueC2.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c3", valueC3.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "c4", valueC4.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "h1", valueH1.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "h2", valueH2.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "kt", valueKt.str());
	genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "emission.gain", emissivity.str());

	OpticalFactorParameters opticalFactorParameters;
	if (PhaseDAO::readCoeffDiffParametersForLambertianOpticalFactors(hapkePropertyParameters.getModelIndex(), opticalFactorParameters)) {
		std::string textureName = "l" + boost::lexical_cast<std::string>(hapkePropertyParameters.getModelIndex()) + "OF";
		genDTO.luxCoreRenderSceneProperties.ajouterCle(keyRoot.str() + "opticalFactors", textureName);

		std::string textureKeyRoot = "scene.textures." + textureName;
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".type", "opticalFactors");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".generator", "phase");
		genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".file", textureName + ".bin");

		if (opticalFactorParameters.sameOpticalFactorForAllBands) {
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", opticalFactorParameters.fileNames[0]);
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", opticalFactorParameters.duplicateFirstLayer[0] ? "1" : "0");
		} else {
			std::ostringstream inputsOSS, duplicationOSS;
			inputsOSS << opticalFactorParameters.fileNames[0];
			duplicationOSS << opticalFactorParameters.duplicateFirstLayer[0];
			for (unsigned int spectralBandIndex = 1; spectralBandIndex < genDTO.numberOfSpectralBands; spectralBandIndex++) {
				inputsOSS << ";" << (opticalFactorParameters.fileNames[spectralBandIndex] == "" ? "1" : opticalFactorParameters.fileNames[spectralBandIndex]);
				duplicationOSS << ";" << (opticalFactorParameters.duplicateFirstLayer[spectralBandIndex] ? "1" : "0");
			}
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputs", inputsOSS.str());
			genDTO.luxCoreRenderSceneProperties.ajouterCle(textureKeyRoot + ".inputsDuplication", duplicationOSS.str());
		}
	}
}

void computeAlbedodTauMoyHapke(	const PropertyHapke & opticalProperties,
								double & albedo,
								double & dTauMoy,
								const vector<DirectionCartesienne> & listDirections) {
	double hapkeGamma = 1.;
	if (opticalProperties.hapkeParameterW > 1.) {
		hapkeGamma = 0.;
	} else if (opticalProperties.hapkeParameterW < 0.) {
		hapkeGamma = 1.;
	} else {
		hapkeGamma = sqrt(1 - opticalProperties.hapkeParameterW);
	}
	albedo = (1. - hapkeGamma) / (1. + hapkeGamma); //Suppression du spéculaire + opticalProperties.albedoSpecular;
	dTauMoy = (PhaseProperty::computeMeanDirectTransmittance(opticalProperties.directTransmittance, listDirections)
			+ opticalProperties.diffuseTransmittance) * 0.5;
}

void testHapkeAlbedo(	const unsigned int& modelIndex,
						PropertyHapke & opticalProperties) {
	if (!opticalProperties.specularData.isSpecular) {
		if (opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance > 1.000001) {
			cerr << " HAPKE Warning : hapke W + DiffuseTransmittance = " << opticalProperties.hapkeParameterW + opticalProperties.diffuseTransmittance
					<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
		}
	} else {
		if (!opticalProperties.specularData._useRefraction) {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					> 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance << " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		} else {
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_down_to_up > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_down_to_up + DiffuseTransmittance + albedo_refraction_down_to_up = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_down_to_up
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_down_to_up
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
			if (opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down + opticalProperties.diffuseTransmittance
					+ opticalProperties.specularData.albedo_refraction_up_to_down > 1.000001) {
				cerr << " HAPKE Warning : hapke W + albedo_up_to_down + DiffuseTransmittance + albedo_refraction_up_to_down = "
						<< opticalProperties.hapkeParameterW + opticalProperties.specularData.albedo_up_to_down
								+ opticalProperties.diffuseTransmittance + opticalProperties.specularData.albedo_refraction_up_to_down
						<< " > 1 !! - lambertian phase function : " << modelIndex << " " << endl;
			}
		}
	}
	if (opticalProperties.hapkeParameterW < 0) {
		cerr << " HAPKE Warning : hapke W < 0 !!! (hapke phase function : " << modelIndex << " ) -> fixed to 0" << endl;
		opticalProperties.hapkeParameterW = 0;
	}
}

void computeOpticalPropertiesHapke(	const unsigned int& spectralBandIndex,
									OpticalPropertyParameters & hapkePropertyParameters,
									const vector<DirectionCartesienne> & listDirections,
									PropertyHapke & hapkeProperty) {
	shared_ptr<HapkeDatabaseParameters> hapkeDatabaseParameters = ::static_pointer_cast<HapkeDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(HAPKE_DATABASE));
	shared_ptr<PhaseTemperatureDatabaseParameters> temperatureDatabaseParameters = ::static_pointer_cast<PhaseTemperatureDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(TEMPERATURE_DATABASE));
	shared_ptr<TransmittanceDatabaseParameters> hapkeTransmittanceParameters = ::static_pointer_cast<TransmittanceDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(TRANSMITTANCE_DATABASE));
	shared_ptr<SpecularDatabaseParameters> kapkeSpecularParameters = ::static_pointer_cast<SpecularDatabaseParameters>(
		hapkePropertyParameters.getDatabaseParameters(SPECULAR_DATABASE));
	unsigned int modelIndex = hapkePropertyParameters.getModelIndex();

	if (kapkeSpecularParameters.get() != NULL) {
		hapkeProperty.specularData.isSpecular = true;
		computeSpecular(hapkeProperty.specularData, PhaseProperty::getMultiplicativeFactorSpecularIntensityForHapke(modelIndex, spectralBandIndex),
			spectralBandIndex, kapkeSpecularParameters);
	} else {
		hapkeProperty.specularData.isSpecular = false;
	}

	hapkeProperty.directTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDirectTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDirectTransmittanceFactorForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.diffuseTransmittance = 0.01 * hapkeTransmittanceParameters.get()->getDiffuseTransmittanceForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorDiffuseTransmittanceFactorForHapke(modelIndex, spectralBandIndex);

	hapkeProperty.temperatureData.directTransmittanceTemperature = temperatureDatabaseParameters->getDirectTransmittanceTemperature();

	hapkeProperty.hapkeParameterW = hapkeDatabaseParameters.get()->getHapkeWForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorWForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC1 = hapkeDatabaseParameters.get()->getHapkeC1ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC1ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC2 = hapkeDatabaseParameters.get()->getHapkeC2ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC2ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC3 = hapkeDatabaseParameters.get()->getHapkeC3ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC3ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterC4 = hapkeDatabaseParameters.get()->getHapkeC4ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorC4ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterH1 = hapkeDatabaseParameters.get()->getHapkeH1ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorH1ForHapke(modelIndex, spectralBandIndex);
	hapkeProperty.hapkeParameterH2 = hapkeDatabaseParameters.get()->getHapkeH2ForBand(spectralBandIndex)
			* PhaseProperty::getMultiplicativeFactorH2ForHapke(modelIndex, spectralBandIndex);
}
