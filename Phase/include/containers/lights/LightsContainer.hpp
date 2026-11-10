/*
 * LightsContainer.hpp
 *
 *  Created on: Jul 5, 2020
 *      Author: lauretn
 */

#ifndef INCLUDE_CONTAINERS_LIGHTS_LIGHTSCONTAINER_HPP_
#define INCLUDE_CONTAINERS_LIGHTS_LIGHTSCONTAINER_HPP_

#include "Light.hpp"

class LightsContainer {
private:
	std::vector<shared_ptr<Light> > lightList;

public:
	void addLight(const Light & newLight) {
		lightList.push_back(newLight.clone());
	}

	unsigned int getNbLights() const {
		return lightList.size();
	}

	const Light & getLight(unsigned int lightIndex) const {
		return *lightList[lightIndex];
	}

	Light & getModifiableLight(unsigned int lightIndex) {
		return *lightList[lightIndex];
	}

	void removeLight(unsigned int lightIndex) {
		lightList.erase(lightList.begin() + lightIndex);
	}
};

#endif /* INCLUDE_CONTAINERS_LIGHTS_LIGHTSCONTAINER_HPP_ */
