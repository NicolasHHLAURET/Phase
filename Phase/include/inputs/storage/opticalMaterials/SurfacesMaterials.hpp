/*
 * SurfacesMaterials.hpp
 *
 *  Created on: 28 sept. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACESMATERIALS_HPP_
#define INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACESMATERIALS_HPP_

#include "lambertian/Lambertian.hpp"
#include "hapke/Hapke1981.hpp"
#include "hapke/Hapke2012.hpp"
#include "rpv/RPV.hpp"
#include "discreteBSDF/DiscreteBSDF.hpp"
#include "specular/Specular.hpp"
#include "mixed/Mixed.hpp"

namespace phsdata {
class SurfacesMaterials {
public:
std::vector<std::shared_ptr<SurfaceFunction> > surfaceOpticalFunctions;

void loadFromParameters(const std::string& prefix) {
	// Lambertians
	if (DartSAParameters::exists(prefix + ".LambertianMultiFunctions")) {
		const std::string lambertiansPrefix = prefix + ".LambertianMultiFunctions";
		int count = DartSAParameters::getNbOf(lambertiansPrefix + ".LambertianMulti");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<SurfaceFunction>(new LambertianMulti());
			func->loadFromParameters(lambertiansPrefix + ".LambertianMulti[" + std::to_string(i) + "]");
			surfaceOpticalFunctions.push_back(func);
		}
	}
	// Hapke
	if (DartSAParameters::exists(prefix + ".HapkeSpecularMultiFunctions")) {
		const std::string hapkesPrefix = prefix + ".HapkeSpecularMultiFunctions";
		{
			// Hapke 1981
			int count = DartSAParameters::getNbOf(hapkesPrefix + ".HapkeSpecularMulti");
			for (int i = 0; i < count; ++i) {
				auto func = std::shared_ptr<SurfaceFunction>(new Hapke1981());
				func->loadFromParameters(hapkesPrefix + ".HapkeSpecularMulti[" + std::to_string(i) + "]");
				surfaceOpticalFunctions.push_back(func);
			}
		}
		{
			// Hapke 2012
			int count = DartSAParameters::getNbOf(hapkesPrefix + ".HapkeSpecularMulti2");
			for (int i = 0; i < count; ++i) {
				auto func = std::shared_ptr<SurfaceFunction>(new Hapke2012());
				func->loadFromParameters(hapkesPrefix + ".HapkeSpecularMulti2[" + std::to_string(i) + "]");
				surfaceOpticalFunctions.push_back(func);
			}
		}
	}
	// RPV
	if (DartSAParameters::exists(prefix + ".RPVMultiFunctions")) {
		const std::string rpvsPrefix = prefix + ".RPVMultiFunctions";
		int count = DartSAParameters::getNbOf(rpvsPrefix + ".RPVMulti");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<SurfaceFunction>(new RPVMulti());
			func->loadFromParameters(rpvsPrefix + ".RPVMulti[" + std::to_string(i) + "]");
			surfaceOpticalFunctions.push_back(func);
		}
	}
	// Discrete BSDF
	if (DartSAParameters::exists(prefix + ".PhaseExternMultiFunctions")) {
		const std::string dBSDFPrefix = prefix + ".PhaseExternMultiFunctions";
		int count = DartSAParameters::getNbOf(dBSDFPrefix + ".PhaseExtern");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<SurfaceFunction>(new DiscreteBSDF());
			func->loadFromParameters(dBSDFPrefix + ".PhaseExtern[" + std::to_string(i) + "]");
			surfaceOpticalFunctions.push_back(func);
		}
	}
	// Specular
	if (DartSAParameters::exists(prefix + ".SpecularMultiFunctions")) {
		const std::string spcularPrefix = prefix + ".SpecularMultiFunctions";
		int count = DartSAParameters::getNbOf(spcularPrefix + ".SpecularMulti");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<SurfaceFunction>(new Specular());
			func->loadFromParameters(spcularPrefix + ".SpecularMulti[" + std::to_string(i) + "]");
			surfaceOpticalFunctions.push_back(func);
		}
	}
	// Mixed
	if (DartSAParameters::exists(prefix + ".MixedMultiFunctions")) {
		const std::string spcularPrefix = prefix + ".MixedMultiFunctions";
		int count = DartSAParameters::getNbOf(spcularPrefix + ".MixedMulti");
		for (int i = 0; i < count; ++i) {
			auto func = std::shared_ptr<SurfaceFunction>(new Mixed());
			func->loadFromParameters(spcularPrefix + ".MixedMulti[" + std::to_string(i) + "]");
			surfaceOpticalFunctions.push_back(func);
		}
	}
}
};
}

#endif /* INCLUDE_INPUTS_STORAGE_OPTICALMATERIALS_SURFACESMATERIALS_HPP_ */
