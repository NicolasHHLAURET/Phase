/*
 * SurfaceMaterialProcessor.hpp
 *
 *  Created on: 30 oct. 2026
 *      Author: lauretn
 */

#ifndef INCLUDE_MATERIALS_SURFACEMATERIALPROCESSOR_HPP_
#define INCLUDE_MATERIALS_SURFACEMATERIALPROCESSOR_HPP_

#include "PhaseDTOs.hpp"
#include "../../librairieDart/include/mapping/PhaseKeyMappingGenerator.hpp"

namespace phase {
class SurfaceMaterialProcessor {public:
    // A handler performs the full treatment for one material.
    // Returns false if nothing could be processed (LambertianMaterialWriter::treat semantics).
    using MaterialHandler = std::function<
        bool(const phsdata::SurfaceFunction & sf,
            PhaseGeneralParametersDTO & genDTO,
            const std::vector<bool> & useSpectralIrradianceForReflectance,
            unsigned int materialIndex,
            PhaseKeyMappingGenerator & pkmg,
            std::ofstream & outCoeffSpecProperties)>;

    // Registers (or replaces) the handler for a material type.
    // Throws std::invalid_argument if handler is null.
    void registerWriter(phsdata::SurfaceFunction::Type type, MaterialHandler handler);

    // Convenience overload: registers an already-constructed writer and binds
    // its treat-style method. WriterT must expose:
    //   bool treat(const phsdata::SurfaceFunction &, const PhaseGeneralParametersDTO &,
    //              const std::vector<bool> &, unsigned int, PhaseKeyMappingGenerator &, std::ofstream &) const
    template<class WriterT>
    void registerWriter(phsdata::SurfaceFunction::Type type, std::shared_ptr<WriterT> writer) {
        if (!writer) {
            throw std::invalid_argument("SurfaceMaterialProcessor: null writer");
        }
        registerWriter(type,
            [writer](const phsdata::SurfaceFunction & sf,
                    PhaseGeneralParametersDTO & genDTO,
                    const std::vector<bool> & useSpectralIrradianceForReflectance,
                    unsigned int materialIndex,
                    PhaseKeyMappingGenerator & pkmg,
                    std::ofstream & outCoeffSpecProperties) {
                return writer->treat(sf, genDTO, useSpectralIrradianceForReflectance,
                    materialIndex, pkmg, outCoeffSpecProperties);
            });
    }

    // Replaces processSurfaceMaterials: iterates all surface functions,
    // dispatching each to its registered handler (unregistered types are skipped,
    // matching the previous TODO branches). The material index advances for every
    // entry regardless of treatment success, exactly like the original loop.
    void processAll(PhaseGeneralParametersDTO & genDTO,
        PhaseKeyMappingGenerator & pkmg,
        std::ofstream & outCoeffSpecProperties) const;

private:
    std::vector<bool> computeUseSpectralIrradiance(const PhaseGeneralParametersDTO & genDTO) const;

    std::map<phsdata::SurfaceFunction::Type, MaterialHandler> _handlers;
};
} // namespace phase

#endif /* INCLUDE_MATERIALS_SURFACEMATERIALPROCESSOR_HPP_ */
