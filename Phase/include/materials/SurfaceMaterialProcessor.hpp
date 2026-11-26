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

    // LuxCore material name provider per type: name = getMaterialName(indexFunction).
    // Registered automatically by the template registerWriter below.
    using NameProvider = std::function<std::string(unsigned int)>;
    void registerNameProvider(phsdata::SurfaceFunction::Type type, NameProvider provider);

    // Translates the XML/globalReferences type value (typeOpticalLink in
    // Mixed links: 0=Lambertian, 2=Hapke, 4=RPV, 11=Phase Extern, 13=Specular,
    // 22=Mixed, 23=Hapke2) into the phsdata storage type. Exits on unknown.
    static phsdata::SurfaceFunction::Type surfaceTypeFromXmlType(int xmlType);

    // LuxCore material name referenced by a Mixed link: resolves xmlType through
    // the registered writer of that type, so the name is guaranteed to match the
    // one written by that writer's writeLuxCoreMaterial. Exits (-7) if no writer
    // is registered for the type.
    std::string materialNameFor(int xmlType, unsigned int indexFunction) const;

    // Resolves an optical link (Mixed FirstProperty/SecondProperty, Specular
    // OpticalLink: xmlType = typeOpticalLink, index = indexOpticalLink) to the
    // LuxCore material name of the referenced surface material. Exits (-6) on
    // volume links, (-7) if the referenced (type, indexFunction) does not exist
    // in genDTO._surfaceMaterials.surfaceOpticalFunctions or has no registered
    // writer.
    std::string referencedMaterialName(int xmlType, int indexOpticalLink,
        const PhaseGeneralParametersDTO & genDTO) const;

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
        registerNameProvider(type, [](unsigned int indexFunction) {
            return WriterT::getMaterialName(indexFunction);
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
    std::map<phsdata::SurfaceFunction::Type, NameProvider> _nameProviders;
};
} // namespace phase

#endif /* INCLUDE_MATERIALS_SURFACEMATERIALPROCESSOR_HPP_ */
