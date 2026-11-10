/*
 * SpectralIrradiance.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_ILLUMINATION_SPECTRALIRRADIANCE_HPP_
#define INCLUDE_MODEL_RADIATION_ILLUMINATION_SPECTRALIRRADIANCE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "yaml-cpp/yaml.h"
#include "IAtmosphericIlumination.hpp"
#include "../../../object/database/illuminationDatabase/AtmosphericIlluminationDatabase.hpp"
namespace DartInputs {
	class SpectralIrradiance: public IAtmosphericIlumination {
	private:
		shared_ptr<AtmosphericIlluminationDatabase> spectralDatabase;

	public:
		SpectralIrradiance() {}

		const shared_ptr<AtmosphericIlluminationDatabase> & getSpectralDatabase() const {
			return spectralDatabase;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (node.get("spectralDatabase").Tag() == "!object.database.illuminationDatabase.AtmosphericIlluminationDatabase") {
					spectralDatabase = shared_ptr<AtmosphericIlluminationDatabase>(new AtmosphericIlluminationDatabase());
					(*spectralDatabase) = node.get("spectralDatabase");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_ILLUMINATION_SPECTRALIRRADIANCE_HPP_ */
