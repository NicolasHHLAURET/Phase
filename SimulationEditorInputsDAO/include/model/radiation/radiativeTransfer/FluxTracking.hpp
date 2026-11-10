/*
 * FluxTracking.hpp
 *
 *  Created on: 18 mars 2019
 *      Author: guilleuxj
 */

#ifndef INCLUDE_MODEL_RADIATION_FLUXTRACKING_HPP_
#define INCLUDE_MODEL_RADIATION_FLUXTRACKING_HPP_

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "IRadiativeTransferMethod.hpp"
#include "fluxTracking/FluxTrackingParameters.hpp"
#include "temperature/ISceneTemperature.hpp"
#include "temperature/UserDefinedSceneTemperature.hpp"
#include "temperature/VerticalProfile1DSceneTemperature.hpp"
#include "temperature/VerticalProfile3DSceneTemperature.hpp"
#include "temperature/ExtrapolatedFromAtmosphereSceneTemperature.hpp"
#include "illumination/IAtmosphericIlumination.hpp"
#include "illumination/SpectralIrradiance.hpp"
#include "illumination/BandIrradiance.hpp"
#include "illumination/BandRadiance.hpp"
#include "illumination/SpectralRadiation.hpp"
#include "../../atmosphere/atmosphereFluxTracking/AtmosphereFluxTracking.hpp"
#include "imageSideIllumination/ImageSideIllumination.hpp"
#include "yaml-cpp/yaml.h"
namespace DartInputs {
	class FluxTracking: public IRadiativeTransferMethod {
	private:
		shared_ptr<FluxTrackingParameters> fluxTrackingParameters;
		shared_ptr<IAtmosphericIlumination> solarIllumination;
		shared_ptr<ISceneTemperature> sceneTemperature;
		shared_ptr<AtmosphereFluxTracking> atmosphereFluxTracking;
		shared_ptr<ImageSideIllumination> imageSideIllumination;

	public:
		FluxTracking() {}

		const shared_ptr<IAtmosphericIlumination> & getSolarIllumination() const {
			return solarIllumination;
		}

		const shared_ptr<ISceneTemperature> & getSceneTemperature() const {
			return sceneTemperature;
		}

		const shared_ptr<AtmosphereFluxTracking> & getAtmosphereFluxTracking() const {
			return atmosphereFluxTracking;
		}

		const shared_ptr<FluxTrackingParameters> & getFluxTrackingParameters() const {
			return fluxTrackingParameters;
		}

		const shared_ptr<ImageSideIllumination> & getImageSideIllumination() const
		{
			return imageSideIllumination;
		}
		void operator =(const YAML::Node& node) {

			if (!node.IsNull()) {
				if (!node.get("fluxTrackingParameters").IsNull()) {
					fluxTrackingParameters = shared_ptr<FluxTrackingParameters>(new FluxTrackingParameters());
					(*fluxTrackingParameters) = node.get("fluxTrackingParameters");
				}

				if (!node.get("imageSideIllumination").IsNull()) {
					imageSideIllumination = shared_ptr<ImageSideIllumination>(new ImageSideIllumination());
					(*imageSideIllumination) = node.get("imageSideIllumination");
				}

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
				if (!node.get("sceneTemperature").IsNull()) {
					if (node.get("sceneTemperature").Tag() == "!radiation.radiativeTransfer.temperature.UserDefinedSceneTemperature") {
						sceneTemperature = shared_ptr<ISceneTemperature>(new UserDefinedSceneTemperature());
						static_cast<UserDefinedSceneTemperature &>(*sceneTemperature) = node.get("sceneTemperature");
					} else if (node.get("sceneTemperature").Tag() == "!radiation.radiativeTransfer.temperature.VerticalProfile1DSceneTemperature") {
						sceneTemperature = shared_ptr<ISceneTemperature>(new VerticalProfile1DSceneTemperature());
						static_cast<VerticalProfile1DSceneTemperature &>(*sceneTemperature) = node.get("sceneTemperature");
					} else if (node.get("sceneTemperature").Tag() == "!radiation.radiativeTransfer.temperature.VerticalProfile3DSceneTemperature") {
						sceneTemperature = shared_ptr<ISceneTemperature>(new VerticalProfile3DSceneTemperature());
						static_cast<VerticalProfile3DSceneTemperature &>(*sceneTemperature) = node.get("sceneTemperature");
					} else if (node.get("sceneTemperature").Tag() == "!radiation.radiativeTransfer.temperature.ExtrapolatedFromAtmosphereSceneTemperature") {
						sceneTemperature = shared_ptr<ISceneTemperature>(new ExtrapolatedFromAtmosphereSceneTemperature());
						static_cast<ExtrapolatedFromAtmosphereSceneTemperature &>(*sceneTemperature) = node.get("sceneTemperature");
					}
				}
				if (!node.get("atmosphereFluxTracking").IsNull()) {
					atmosphereFluxTracking = shared_ptr<AtmosphereFluxTracking>(new AtmosphereFluxTracking());
					(*atmosphereFluxTracking) = node.get("atmosphereFluxTracking");
				}
			}
		}
	};
}
#endif /* INCLUDE_MODEL_RADIATION_FLUXTRACKING_HPP_ */
