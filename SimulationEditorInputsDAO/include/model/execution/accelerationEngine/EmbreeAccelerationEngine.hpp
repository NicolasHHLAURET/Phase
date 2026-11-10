/*
 * EmbreeAccelerationEngine.hpp
 *
 *  Created on: 21 août 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_EMBREEACCELERATIONENGINE_HPP_
#define INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_EMBREEACCELERATIONENGINE_HPP_

#include "yaml-cpp/yaml.h"
#include "IAccelerationEngine.hpp"
namespace DartInputs {
	class EmbreeAccelerationEngine: public IAccelerationEngine {
	private:
		int nbTrianglesWithinVoxelAcceleration;
		bool sparseVoxelAcceleration;
	public:
		EmbreeAccelerationEngine() :
				nbTrianglesWithinVoxelAcceleration(10), sparseVoxelAcceleration(true) {}

		const int & getNbTrianglesWithinVoxelAcceleration() const {
			return nbTrianglesWithinVoxelAcceleration;
		}
		const bool & getSparseVoxelAcceleration() const {
			return sparseVoxelAcceleration;
		}

		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				nbTrianglesWithinVoxelAcceleration = node.get("nbTrianglesWithinVoxelAcceleration").as<int>();
				sparseVoxelAcceleration = node.get("sparseVoxelAcceleration").as<bool>();
			}
		}
	};
}

#endif /* INCLUDE_MODEL_EXECUTION_ACCELERATIONENGINE_EMBREEACCELERATIONENGINE_HPP_ */
