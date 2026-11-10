/*
 * DartAtmosphere.hpp
 *
 *  Created on: 27 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_DARTATMOSPHERE_HPP_
#define INCLUDE_MODEL_DARTATMOSPHERE_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "atmosphere/atmosphereGeometry/AtmosphereGeometry.hpp"
#include "atmosphere/AerosolModel.hpp"
#include "atmosphere/IAtmosphereModel.hpp"
#include "atmosphere/DatabaseAtmosphereModel.hpp"
#include "atmosphere/UserDefinedAtmosphereModel.hpp"

#include <vector>
#include "../yaml/YaMLReader.hpp"
namespace DartInputs {
	class DartAtmosphere {
	private:
		bool airWithinScene;
		shared_ptr<double> heightOfEarthScene; //!\ Il peut être null  si non défini

		shared_ptr<IAtmosphereModel> atmosphereModel;
		shared_ptr<AtmosphereGeometry> atmosphereGeometry;
		std::vector<shared_ptr<AerosolModel> > aerosolsModels;

	public:
		const bool & getAirWithinScene() const {
			return airWithinScene;
		}

		const shared_ptr<double> & getHeightOfEarthScene() const {
			return heightOfEarthScene;
		}

		const shared_ptr<AtmosphereGeometry> & getAtmosphereGeometry() const {
			return atmosphereGeometry;
		}

		const shared_ptr<IAtmosphereModel> & getAtmosphereModel() const {
			return atmosphereModel;
		}

		const std::vector<shared_ptr<AerosolModel> > & getAerosolsModels() const {
			return aerosolsModels;
		}

		void loadFromYaml(const YaMLReader & yamlReader) {
			airWithinScene = yamlReader.getFirstNode("airWithinScene").as<bool>();
			if (!yamlReader.getFirstNode("heightOfEarthScene").IsNull()) {
				heightOfEarthScene = shared_ptr<double>(new double());
				(*heightOfEarthScene) = yamlReader.getFirstNode("heightOfEarthScene").as<double>();
			}
			if (!yamlReader.getFirstNode("atmosphereGeometry").IsNull()) {
				atmosphereGeometry = shared_ptr<AtmosphereGeometry>(new AtmosphereGeometry());
				(*atmosphereGeometry) = yamlReader.getFirstNode("atmosphereGeometry");
			}

			if (!yamlReader.getFirstNode("atmosphereModel").IsNull()) {
				if (yamlReader.getFirstNode("atmosphereModel").Tag() == "!atmosphere.DatabaseAtmosphereModel") {
					atmosphereModel = shared_ptr<IAtmosphereModel>(new DatabaseAtmosphereModel());
					static_cast<DatabaseAtmosphereModel &>(*atmosphereModel) = yamlReader.getFirstNode("atmosphereModel");
				} else if (yamlReader.getFirstNode("atmosphereModel").Tag() == "!atmosphere.UserDefinedAtmosphereModel") {
					atmosphereModel = shared_ptr<IAtmosphereModel>(new UserDefinedAtmosphereModel());
					static_cast<UserDefinedAtmosphereModel &>(*atmosphereModel) = yamlReader.getFirstNode("atmosphereModel");
				}
			}
			const YAML::Node & nodeAero = yamlReader.getFirstNode("aerosolsModels");
			if (!nodeAero.IsNull() && nodeAero.IsSequence()) {
				for (YAML::const_iterator it = nodeAero.begin(); it != nodeAero.end(); ++it) {
					shared_ptr<AerosolModel> aerosolsModel = shared_ptr<AerosolModel>(new AerosolModel());
					(*aerosolsModel) = (*it);
					aerosolsModels.push_back(aerosolsModel);
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_DARTATMOSPHERE_HPP_ */
