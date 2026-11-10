/*
 * SideIllumination.hpp
 *
 *  Created on: 22 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_SIDEILLUMINATION_HPP_
#define INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_SIDEILLUMINATION_HPP_

#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class SideIllumination {
	private:
		std::string sideIlluminationFileName;
		bool disableOtherSideConnection;
		bool storeExitingEnergy;

	public:
		SideIllumination() : disableOtherSideConnection(false), storeExitingEnergy(false){}

		const std::string & getSideIlluminationFileName() const {
			return sideIlluminationFileName;
		}

		const bool & getDisableOtherSideConnection() const {
			return disableOtherSideConnection;
		}

		const bool & getStoreExitingEnergy() const {
			return storeExitingEnergy;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("sideIlluminationFileName").IsNull()) {
					sideIlluminationFileName = node.get("sideIlluminationFileName").get("dartFilePath").get("relativePath").as<std::string>();
				}
				disableOtherSideConnection = node.get("disableOtherSideConnection").as<bool>();
				storeExitingEnergy = node.get("storeExitingEnergy").as<bool>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_RADIATION_RADIATIVETRANSFER_IMAGESIDEILLUMINATION_SIDEILLUMINATION_HPP_ */
