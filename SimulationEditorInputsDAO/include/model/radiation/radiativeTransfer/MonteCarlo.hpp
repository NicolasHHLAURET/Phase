/*
 * MonteCarlo.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_MONTECARLO_HPP_
#define INCLUDE_MODEL_RADIATION_MONTECARLO_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "monteCarlo/MonteCarloParameters.hpp"
#include "illumination/IAtmosphericIlumination.hpp"
#include "illumination/SpectralIrradiance.hpp"
#include "illumination/BandIrradiance.hpp"
#include "illumination/BandRadiance.hpp"
#include "illumination/SpectralRadiation.hpp"
#include "IRadiativeTransferMethod.hpp"
#include "imageSideIllumination/ImageSideIllumination.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class MonteCarlo: public IRadiativeTransferMethod {
	private:
		shared_ptr<IAtmosphericIlumination> solarIllumination;
		shared_ptr<MonteCarloParameters> monteCarloParameters;
		shared_ptr<ImageSideIllumination> imageSideIllumination;

	public:
		const shared_ptr<IAtmosphericIlumination> & getSolarIllumination() const {
			return solarIllumination;
		}

		const shared_ptr<MonteCarloParameters> & getMonteCarloParameters() const {
			return monteCarloParameters;
		}
		const shared_ptr<ImageSideIllumination> & getImageSideIllumination() const
		{
			return imageSideIllumination;
		}
		void operator =(const YAML::Node& node) {
			if (!node.IsNull()) {
				if (!node.get("solarIllumination").IsNull()) {
					if (node.get("solarIllumination").Tag() == "!radiation.radiativeTransfer.illumination.SpectralIrradiance") {
						solarIllumination = shared_ptr<IAtmosphericIlumination>(new SpectralIrradiance());
						static_cast<SpectralIrradiance &>(*solarIllumination) = node.get("solarIllumination");
					} else if (node.get("solarIllumination").Tag() == "!radiation.radiativeTransfer.illumination.BandIrradiance") {
						solarIllumination = shared_ptr<IAtmosphericIlumination>(new BandIrradiance());
						static_cast<BandIrradiance &>(*solarIllumination) = node.get("solarIllumination");
					} else if (node.get("solarIllumination").Tag() == "!radiation.radiativeTransfer.illumination.BandRadiance") {
						solarIllumination = shared_ptr<IAtmosphericIlumination>(new BandRadiance());
						static_cast<BandRadiance &>(*solarIllumination) = node.get("solarIllumination");
					} else if (node.get("solarIllumination").Tag() == "!radiation.radiativeTransfer.illumination.SpectralRadiation") {
						solarIllumination = shared_ptr<IAtmosphericIlumination>(new SpectralRadiation());
						static_cast<SpectralRadiation &>(*solarIllumination) = node.get("solarIllumination");
					}
				}

				if (!node.get("imageSideIllumination").IsNull()) {
					imageSideIllumination = shared_ptr<ImageSideIllumination>(new ImageSideIllumination());
					(*imageSideIllumination) = node.get("imageSideIllumination");
				}

				if (!node.get("monteCarloParameters").IsNull()) {
					if (node.get("monteCarloParameters").Tag() == "!radiation.radiativeTransfer.monteCarlo.MonteCarloParameters") {
						monteCarloParameters = shared_ptr<MonteCarloParameters>(new MonteCarloParameters());
						(*monteCarloParameters) = node.get("monteCarloParameters");
					}
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_MONTECARLO_HPP_ */
