# CONTEXT — working notes for the Phase modernization sessions

This file is the handoff between working sessions. Update it at the end of each
session. Sections marked STATE reflect the last verified build.

## Goal

Refactor this old C/C++ DART module ("Phase", phase-function computation for
radiative transfer) into pure, modern C++17, open-source ready. XML input
reading has been reorganized (new phsdata::* storage classes, loadFromParameters
pattern) and must be completed. The Dart support libraries will later be fused
and their dead code removed — but only after the main modules are rewritten.

Decisions taken by the owner:
- SimulationEditorInputsDAO is DROPPED (YAML compatibility not maintained).
  The whole YaML path was removed from Phase.
- NetCDF support is deferred: all NetCDF/DartNetcdf usage is gated behind the
  CMake option DART_WITH_NETCDF (default OFF). Atmosphere "without database"
  prints an error when built without it.
- Libraries stay separate for now; dead-code removal comes last.
- Patch-based delivery: sessions have no GitHub connector and cannot push
  (anonymous clone/fetch of this public repo works, push does not), so changes
  are delivered as patches pasted in chat. Follow the "Patch transfer
  protocol" section at the end of this file: small PLAIN-TEXT patches verified
  by `git apply --check` + post-am tree hash, a push-back repair loop on any
  mismatch, and chunked base64 only as a fallback.

## Build state (last verified)

Full chain builds and links with GCC 14, C++17:
dart_base, dart_geometry, dart_env, dart_document (all static libs)
dart_core (librairieDart subset), phase (executable).

Configure/build:
    cmake -S . -B build -DDART_DEPS_PREFIX=<prefix>
    cmake --build build -j8

DART_DEPS_PREFIX is a user-space prefix containing:
- Boost 1.84: headers + ONLY these compiled libs: filesystem, system, thread,
  regex, date_time (b2 headers + b2 --with-... install)
- pugixml, yaml-cpp, SQLite (from sqlite.org mirror), SOCI 4.2 (sqlite3
  backend; needs its fmt submodule and -DSOCI_* to disable other backends)

Sandbox notes for rebuilding the prefix (no root available):
- No distro apt mirrors; git clone from github works; curl does NOT.
- Boost: git clone --recursive --depth 1 -boost-1.84.0 boostorg/boost,
  ./bootstrap.sh (WITHOUT --with-libraries=uuid: it breaks later b2 calls),
  ./b2 headers, then ./b2 --with-filesystem --with-system --with-thread
  --with-regex --with-date_time --prefix=<prefix> install
- Background processes are KILLED between tool calls: run long builds in the
  foreground of a single command.

## History of the rework (what was done, in order)

1. Modern top-level CMakeLists.txt (C++17, find_package, incremental module
   inclusion: modules without CMakeLists.txt are skipped with a message).
   Per-library CMakeLists for libDartBase/Geometry/Env/Document, librairieDart
   (dart_core subset), Phase.
2. Removed all dynamic exception specifications (throw(...)) — illegal in C++17.
   Removed the unused `#define byte uint8_t` (GeneralDef.hpp).
3. DartXML ported from TinyXML/TinyXPath to pugixml (TinyXPath unreachable;
   only S_xpath_string/o_xpath_string were used). Unused XPath accessors
   dropped. Fixed pre-existing bug: DartXML::set() passed newKey instead of
   key to formatKey().
4. CRLF files remaining (7, e.g. DartException family, MakInversionParser):
   empty `throw()` specs left in place (legal, deprecated). Normalizing
   CRLF→LF is a wanted later cleanup.
5. Phase: YAML path stripped (PhaseDAO _useYaML/getInputsDAO, YaMLContainer
   files deleted, "yaml" CLI option removed). Dead relative SOCI includes
   replaced by <soci/soci.h> / <soci/sqlite3/soci-sqlite3.h>.
6. Foliar fluorescence: the free functions computeFluorescenceMatrices /
   writeFluorescenceToSimulationProperties were declared+called but NEVER
   defined (in any commit). Ported to the new phase::FluorescenceService via
   computeFoliarFluorescenceMatrices() in Phase.cpp — a bridge converting the
   legacy FluorescenceParameters struct into phsdata::ProspectExternParameters /
   FluorescenceExternParameters. OWNER: please review the field mapping
   (chlorophyll→cab, structureCoeff→n, useNewCm→useCm, ...).
7. phsdata storage fixes: legacy EmissionMaterials::VoxelNumber renamed
   EmissionVoxelNumber (clashed with new phsdata::VoxelNumber);
   MultiplicativeFactorList::loadFromParameters made virtual (override);
   FoliarComputation::_numberOfAngularSectors member added;
   CSVReader::getDouble gained its missing key parameter;
   never-defined computePhaseFunctionForAngularSector call removed.
8. Direction sets: new phsdata::UnderstoryMulti storage (vegetation) with
   DirectionSetLink (unsigned int indexDirectionSetLink), new
   phsdata::VolumesMaterials container, parsed from Materials.Volumes in
   PhaseDAO::parseOpticalMaterials, stored in genDTO._volumesMaterials.
9. processVegetationVolumeMaterials (PhaseVegetation.cpp) completed: returns
   VegetationVolumeMaterials (unique_ptrs to FoliarModel/FoliarComputation/
   LADComputation — they are non-copyable, hold references — plus fTFoliar,
   point lists, per-band sun-irradiance flags). Caller writes:
   writePhasePropertiesSpectralIrradiance(outCoeffSpecProperties, pkmg, genDTO).
   Direction set resolved by getVegetationDirectionSetIndex(): first valid
   UnderstoryMulti link drives the SHARED precomputing. TRUE per-model
   direction sets still need per-model support in FoliarModel/FoliarComputation.
   testIfUseSun RESOLVED in item 26: the "spectralDomain != Thermal" variant
   below was authored at 30ea479 on the broken initial commit cfa3288
   (dangling call in src/PhaseVegetation.cpp:35, no definition in the tree);
   the PhaseOld rule minus the LiDAR term was restored instead (OWNER
   confirmed).
10. misc: Hapke2012/RPV writer constructors defined; header-defined functions
    marked inline (DartXML::getStringWithPossibleWhiteSpace, luxcore helpers,
    CSVReader::split) — multiple-definition link errors otherwise;
    libDependencies/memory_tools.hpp (missing lib) replaced by self-contained
    getTotalSystemMemory/getAvailableSystemMemory (sysconf/_WIN32);
    extern "C" int sqlite3_initialize(void); declaration only — SOCI declares
    a global VARIABLE named sqlite3 which clashes with the sqlite3 TYPE if
    sqlite3.h is included after soci headers;
    atmospherePhaseFunction/computeAndWritePolarizationMatrixAtmosphere call
    sites updated to new sunDirection signatures; processSurfaceMaterials and
    OpticalFactorsTextureWriter take non-const genDTO (they write LuxCore keys);
    self-destruct database list rebuilt via createDatabasesReadingProperties.
11. Foliar prospect generation ported to the phsdata storage (was the open TODO):
    ProspectExternParameters/ProspectExternalModule/FluorescenceExternParameters
    moved from lambertian/Lambertian.hpp to the shared
    opticalMaterials/ProspectExternalModule.hpp; new phsdata::UnderstoryModel
    (modelName, databaseName, prospectExternalModule) and extended
    phsdata::UnderstoryMulti (hasDifferentModelForBottom, functionName,
    indexFunction, UnderstoryMultiModel/Top/Bottom optional nodes,
    getTopModel()/getBottomModel()); prepareProspect() now iterates
    genDTO._volumesMaterials.understoryMultiFunctions and stores the generated
    uuid database name in each face's databaseName (trigger mirrors Lambertian:
    useProspectExternalModule || isFluorescent, externParameters required);
    createDatabasesReadingProperties() takes genDTO and builds the foliar
    entries from the struct (specular foliar data dropped -- old XML handled by
    the external upgrader); static mapPrepareProspectFoliarTop/Bottom and the
    dead PhaseDAO readers (readCoeffDiffParametersForIsProspectUseFoliar,
    readCoeffDiffParametersForFoliarDatabases(XML),
    readCoeffDiffParametersForProspect(XML)) removed;
    processVegetationVolumeMaterials takes numberOfFoliarModels from
    genDTO._volumesMaterials.
12. Patch-transfer repair validated on the foliar patch (this session):
    the plain-text patch arrived with 2 whitespace-only lines DELETED
    (structural "corrupt patch at line 457") plus tab mangling on
    context/removed lines; after reinserting the 2 dropped lines and
    `git am --ignore-whitespace`, the post-am tree hash matched exactly
    (0a98de5222a12372f4f27e22b73f753bdc5f9a9b on top of ce3c619; receiver
    evidence in transfer/failed/, applied in commit 965fa10). See new
    pitfall (d) and protocol point 5.
13. Per-model direction sets + treatFoliar rewrite (this session): new
    phase::VegetationMaterialWriter (Phase/materials/) ports the old treatFoliar
    body to the phsdata structs; models are iterated grouped by DirectionSetLink
    (stable order within a group, output model index = UnderstoryMulti::
    indexFunction) and prepareVegetationDirectionSet() (PhaseVegetation.cpp)
    redoes the direction-set-dependent precomputing (init + computeLadData +
    readMultiplicativeFactor + computeFoliarCosinePsi + foliarPrecomputing)
    only when the direction set changes. processVegetationVolumeMaterials() now
    only allocates the shared FoliarModel/FoliarComputation/LADComputation and
    computes useSpectralIrradianceForReflectance. The sphereMap line lost during
    the earlier extraction is restored as a writer member, built once from
    fTFoliar.iterationStepPhi (SphereDiscretizationMap(1.0, deltaPhi) does not
    depend on the direction set). buildFoliarOpticalPropertyParameters()
    (PhaseVegetation.cpp) is shared by createDatabasesReadingProperties() and
    the writer. Old treatFoliar, computeFoliarFluorescenceMatrices,
    computeFoliarSpecular and the commented treat* loop in Phase.cpp removed.
    Foliar specular database dropped (external XML upgrader handles old inputs).
    Bug fix: LambertianMaterialWriter::prepareDatabase() now uses
    DartEnv::getOutputTempDatabasePath(true) + selfDestruct for prospect-
    generated databases (it wrongly used getDatabasePath -> wrong folder).
14. Vegetation-writer patch delivery (same session, owner-applied as
    015c6e8; post-am tree vs 37a77a6 =
    5f7c0e047d2b798e75c0b8ae9954f5314463cbf7, `git diff -w` vs the sender's
    commit b35bea3 empty -- the owner accepted the 32 whitespace-only-damaged
    added lines, cosmetic rework planned): the repair loop ran a second time
    with NEW structural corruption modes (pitfalls (e)/(f)): a split context
    line, a dropped "-" line, a junk char on an @@ header, a marker flip
    " " -> "-", backslash insertions before quotes in removed lines, and
    sender-side CASE FLIPS inside the transmitted repair script (8x
    useBioClimaticProfileEta instead of the code's useBioCLimaticProfileEta)
    which passed `git apply --check` but failed `git am` at Phase.cpp:1514.
    The corrected repair script was then sent base64-encoded with the
    decoded-script sha256 as the gate, and the patch applied with
    `git am --ignore-whitespace`.
15. BioClimatic typo fix (owner decision, after the vegetation-writer
    patch): the identifier useBioCLimaticProfileEta (capital L) renamed to
    useBioClimaticProfileEta in all 6 code files -- 16 occurrences including
    the generated scene-property key strings ("useBioCLimaticProfileEta" and
    the "topUse..."/"bottomUse..." variants in the material writers); no XML
    input references these keys. Note: pitfall (f) and item 14 above still
    quote the pre-rename spelling (historical incident description).
16. Per-model LAD (this session): the LAD and g-function vectors of FoliarModel
    collapsed from [nbModels][...] to single slots, and LADComputation is now
    stateless per model (only a scratch PDF buffer; the unused
    _transmTransfertFunctionVector member reference dropped). computeLadData
    (which looped over ALL models at direction-set preparation) is replaced by
    computeLadDataForModel(), called from VegetationMaterialWriter::treat() for
    the model being treated only. Index-confusion fix: PhasePolarization and
    computeAndWriteAverageBioClimaticProfile now use the model POSITION
    (foliarIndice) for all FoliarModel/LAD data access; the XML indexFunction
    (getModelIndex()) is used only for output naming. LADComputation methods
    lost their modelIndex parameter (writeLad keeps one, for the output file
    name only).
17. Per-model LAD patch delivery (same session): owner-applied as a45629a,
    tree 549048aa5bce3f61089ec33cb93db22a896594bc instead of the sender's
    2293cda4... -- classified BENIGN: 1047/1047 lines arrived (no structural
    damage), 353 lines with leading-indentation damage only (315 tab->space,
    38 tabs dropped), `git diff -w` vs the sender's commit empty. New
    pitfall variant noted in (a); failed transfer files now live in the
    dedicated AICommunications repo (protocol 3/5, checklist 4).
18. PhasePolarization LAD recompute removed (this session): both
    computeAndWriteFoliarPolarizationMatrix{,NotBinary} no longer re-read the
    LAD parameters nor recompute the LAD per spectral band -- the LAD of the
    model being treated is computed once by computeLadDataForModel() in
    VegetationMaterialWriter::treat(). The foliarIndice and ladComputation
    parameters (and the LADComputation include) were dropped from both
    functions; behavior unchanged (same model, same parameters, single-slot
    LAD vectors).
19. Foliar LAD params ported to phsdata (this session, templates in
    Phase/inputTest/XMLTemplate v5+v6 used as the exhaustive reference):
    phsdata::UnderstoryMulti now loads lad, dimFoliar, thermalHotSpotFactor,
    DirectionalClumpingIndexProperties (clumpinga/clumpingb/omegaMin/omegaMax)
    and the conditional LAD children (Ellipsoidal.ala, Elliptical.thetam/
    epsilon, BoundedUniform.nbAla/nthAla, UserDefined.fileName,
    Manual.lad_m/lad_b0/lad_b1/lad_b2) via exists() guards.
    computeLadDataForModel() now takes the UnderstoryMulti and copies these
    into the per-model FoliarModel params (no PhaseDAO read); the DAO functions
    readCoeffDiffParametersForFoliar{,XML} are deleted. v5->v6 renames handled
    by the external XML upgrader, not here.
20. Foliar optical factors ported to phsdata + written by Phase itself (this
    session): new phsdata::UnderstoryMultiplicativeFactor{,List} (mirrors the
    lambertian ones, with leafTransmittanceFactor), loaded in
    UnderstoryModel as nodeMultiplicativeFactor from
    UnderstoryNodeMultiplicativeFactorForLUT. VegetationMaterialWriter::treat()
    now writes the v<foliarIndice>OF texture via
    OpticalFactorsTextureWriter::writeTexture() (like the surface writers;
    previously written by another module) and the equivalent-lambertian
    materials reference the SAME foliar texture -- fixing the bug where the
    old code read the LAMBERTIAN optical factors with the foliar model index.
    PhaseDAO::readCoeffDiffParametersForFoliarOpticalFactors deleted.
    NOTE: duplicateFirstMatrixLayer in the v6 template is a producer-side
    error (to be dropped from the XML); phsdata deliberately does NOT read it.
21. TODO section consolidated (this session, agreed with owner): added the
    open points below (Foliar C-structure cleanup, treatSpecular/
    treatPhaseExtern/treatMixedMaterial refactorization, treatHapke dead code,
    remaining PhaseDAO call sites, database self-destruct, foliar OF
    Top-vs-Bottom). PhaseOld/ (added in 9dc5a2f) holds the pre-factorization
    code for old-vs-new comparison and validation.
22. Foliar multiplicative factors ported to phsdata (this session): the last
    foliar PhaseDAO read is gone. phsdata::VolumesMaterials now loads
    integrationStepOnTheta/Phi (defaults 1/10, like the old DAO) and snaps
    them to the divisor tables via snapToNearestDivider -- verified
    equivalent to the old DAO snapping for all integer values -5..500.
    phsdata::UnderstoryModel loads the useMultiplicativeFactorForLUT flag.
    The v5-only diffuseTransmittanceAcceleration attribute is no longer read
    (dropped in the v6 template): the FoliarModel acceleration factors stay
    at the initFoliarModel default 0, and will disappear with the FoliarModel
    C-structure cleanup. New fillFoliarModelMultiplicativeFactors()
    (PhaseVegetation.cpp) fills the per-model FoliarModel adaxial/abaxial
    reflectance and leaf transmittance from each UnderstoryMulti's top/bottom
    nodeMultiplicativeFactor, iterating the per-band factors one-to-one with
    the spectral bands (producer guarantee documented in
    MultiplicativeFactor.hpp; same convention as the lambertian writer).
    readMultiplicativeFactor() (Phase.cpp) lost its foliar call and its
    step/foliarModel out-params; prepareVegetationDirectionSet() takes the
    snapped steps from genDTO._volumesMaterials. PhaseDAO::
    readMultiplicativeFactorFoliar{,XML} and
    readFoliarMultiplicativeFactorForModelXML are deleted. BEHAVIOR FIX
    (agreed with owner): the old readFoliarMultiplicativeFactorForModelXML
    shadowed its modelKey parameter with a local, so with
    hasDifferentModelForBottom=1 the bottom face was filled from the TOP
    model's node; the port reads the actual Bottom model
    (UnderstoryMultiBottomModel) for the abaxial side, falling back to the
    top model when hasDifferentModelForBottom=0. v6 node/attr names:
    UnderstoryNodeMultiplicativeFactorForLUT /
    UnderstoryMultiplicativeFactorForLUT / leafTransmittanceFactor.
23. Foliar C-structure cleanup (this session): FoliarModel now stores ONE
    FoliarModelParameters slot for the current model (the LAD/g-function
    vectors were already single-slot since item 16); all setters/getters lost
    their model-index parameter. Per-model data is computed only when the
    model is treated: computeLadDataForModel() and the new per-model
    fillFoliarModelMultiplicativeFactors(understory, foliarModel) are both
    called from VegetationMaterialWriter::treat() (the fill moved out of
    prepareVegetationDirectionSet, which no longer loops the models).
    FoliarComputation's _preDiffuseTransfertFunctionVector lost its
    [modelIndex] level (single slot [4][dir][dir]) and now references the
    single FoliarModelParameters slot. This removes the last
    indexFunction-vs-position mixup: computeFoliarPerBand used to read
    _parametersForFoliarModels[foliarPropertyParameters.getModelIndex()]
    (= XML indexFunction) while the setters wrote at the loop position --
    latent out-of-bounds when indexFunction != position (e.g. template
    default -1). Removed dead code: struct fields hasDifferentModelForBottom,
    typeDiffPhase, tMinLeaf/tMaxLeaf/standardDeviationVector,
    diffuseTransmittanceAccelerationFactor/bottom... (v6 dropped the XML
    attribute in item 22; the FoliarProperty fields were write-only), the
    _hotSpotFactorDistributionFunctionVector member + accessors, the unused
    reflectance/transmittance/acceleration getters, and the dead free
    resizePhaseParametersVectors declaration (old Phase.hpp:165).
    initFoliarModel(numberOfSpectralBands, numberOfDirections) only resets
    the common (direction-set) vectors and the single slot now.
24. CONTEXT.md protocol/pitfall update (this session, agreed with owner):
    plain-text patch chunks are sent in BATCHES OF AT MOST 4 CHUNKS PER
    MESSAGE (session memory limits -- observed on the 12-chunk item-23
    patch); new pitfall (g) recorded (duplicated bare "-" lines, dropped
    whitespace-only context lines at chunk seams); receiver command
    checklist added as protocol point 7; the `git diff -w` empty gate is
    documented for trees that differ only by whitespace (item 14/23
    precedent -- the authoritative tree-hash gate then reports a different
    hash, e.g. 608ca5ff vs 0e9bfb0a for item 23).
25. VegetationVolumeMaterials struct deleted (this session): it only provided
    lifetime grouping -- FoliarComputation/LADComputation hold references into
    FoliarModel, so the three must be co-owned with foliarModel destroyed last.
    processVegetationVolumeMaterials() was useless (only allocated the objects +
    testIfUseSun); its content moved into VegetationMaterialWriter::processAll(),
    which now declares the foliar objects as locals (FoliarModel foliarModel;
    FoliarComputation foliarComputation(foliarModel); LADComputation
    ladComputation(foliarModel); FTFoliar fTFoliar; useSpectralIrradiance...).
    They CANNOT be locals of treat(): treat() runs per model while
    prepareVegetationDirectionSet() (called from processAll on direction-set
    change) initializes the shared per-direction-set state they reuse. The dead
    pointsListSpec/pointsListDiff members went away with the struct (treat()
    already had its own locals). processAll/treat/prepareVegetationDirectionSet
    now take the individual refs instead of the struct; the Phase.cpp call site
    is just vegetationWriter.processAll(genDTO, pkmg, outCoeffSpecProperties).
26. testIfUseSun RESTORED to the PhaseOld rule minus the LiDAR term (this
    session, owner decision): useSpectralIrradianceForReflectance[band] =
    PhaseProperty::getWeightingOfReflectanceParameters() &&
    PhaseProperty::getIrradianceFromDatabaseComputationMethod() == 0.
    getCalculatorMethod() != 2 dropped: in v6 LiDAR is its own light source
    and the user controls sun weighting via weightReflectanceParameters.
    VegetationMaterialWriter.cpp testIfUseSun now matches
    SurfaceMaterialProcessor::computeUseSpectralIrradiance exactly.
    Diagnosis (git archaeology): PhaseOld/src/Phase.cpp:962 had the full
    PhaseOld rule (weighting && calculatorMethod != 2 && irrFromDB == 0)
    shared by surface and foliar; public cfa3288 was already broken (call,
    no definition); the Thermal-domain variant was written at 30ea479 and
    recorded as "OWNER: confirm" in item 9. Regression vs PhaseOld, now
    fixed, in 3 cases that used to sun-weight foliar reflectance anyway:
    weighting disabled, LiDAR mode, non-database irradiance.
27. Free foliar helpers moved from Phase.hpp / Phase.cpp / PhaseVegetation.cpp
    into VegetationMaterialWriter.cpp as file-static functions (this session,
    owner request): getUnderstoryDirectionSetIndex,
    fillFoliarModelMultiplicativeFactors (+ its static helper
    fillFoliarModelFaceMultiplicativeFactors), prepareVegetationDirectionSet,
    computeLadDataForModel, readBioClimaticFile,
    computeAndWriteAverageBioClimaticProfile. Each had VegetationMaterialWriter
    as its only caller (verified repo-wide, PhaseOld excluded).
    buildFoliarOpticalPropertyParameters STAYS public in PhaseVegetation.cpp:
    also called from Phase.cpp createDatabasesReadingProperties(). Dead
    declarations removed from Phase.hpp: computeAverageEta and the free
    writeFoliar (no definition, no callers -- the writer calls the
    FoliarComputation::writeFoliar method). Note: foliarPrecomputing /
    numberOfAngularSectors / printNumberOfFoliarModels are still declared in
    Phase.hpp and defined in Phase.cpp; their only caller is now the moved
    prepareVegetationDirectionSet() -- candidates for a later pass.
    readMultiplicativeFactor stays (5 surface call sites in Phase.cpp).
28. _tModulo zero-division fix (this session, owner chose option b): after the
    owner removed the setParametersForPrintingProgress() call from
    prepareVegetationDirectionSet (commit c8ca7fd), _tModulo had no caller
    left and stayed 0, so checkProgress() (PhaseProperty.hpp:170) would
    divide by zero (SIGFPE) on the first reflectance DB read. Fixed by
    initializing _tModulo = 1 (PhaseProperty.cpp). Note the bug was latent
    before too: the only caller lived in the vegetation path, which runs
    only when vegetation models exist (surfaces are processed after, at
    Phase.cpp:2141 vs 2084). See the MODULE PROGRESS REPORT TODO.
29. treatSpecular converted into phase::SpecularMaterialWriter (this session):
    new Phase/include/materials/SpecularMaterialWriter.{hpp,cpp} modeled on
    RPVMaterialWriter, registered in processSurfaceMaterials for
    phsdata::SurfaceFunction::Type::Specular (specular entries were previously
    silently skipped: treatSpecular had no callers). Data now comes from
    phsdata::Specular: refractiveIndex databaseName + ni/nt model names,
    roughnessFactor, opticalDepth (tau), materialCharacter, OpticalLink (base
    BSDF), SpecularNodeMultiplicativeFactorForLUT (ni/nt/opticalDepth/refl/trans
    factors). Old treatSpecular + computeOpticalPropertiesSpecular deleted from
    Phase.cpp/Phase.hpp. RPV copy-paste remnants dropped: commented writeRPV
    call, unused R0/dTauMoy/albedo, v5 updatePropertyForOpticalPropertyInCoeffDiff-
    XmlFile call, hardcoded temperature DB, useDirectionalDirectTransmittance
    block (the v6 template removed that attribute). OF texture now
    "specular<i>OF" via OpticalFactorsTextureWriter (v6 SpecularMultiplicative-
    FactorForLUT owns an OpticalFactorMatrix subtree) instead of the v5
    Lambertian "l<i>OF" reuse. Emissivity on the extra simulated bands
    (Q,U,V / PSI,PSII) is now zero-padded like the other writers (legacy padded
    with 1). The "notImpementedYet" base-material literal is kept for output
    parity.
30. CONTEXT.md rules added (this session, owner request): protocol point 8
    (corrections fix only harmful errors; whitespace/tabulation mangling is
    benign and covered by --ignore-whitespace + tree/diff-w gates);
    handoff items 7 (diagnostic first, ask owner before coding) and 8
    (sandbox /workspace+/tmp are ephemeral -- wipeout observed 2026-11-26;
    the pushed repo is the only durable state, keep changes committed so
    patches are regenerable).

## Known TODOs / open points (next sessions)

- EQUIVALENT-LAMBERTIAN FROM VEGETATION: DONE (this session). treatFoliar now
  builds front/back PropertyLambertian vectors and calls the public
  LambertianMaterialWriter::writeLuxCoreMaterial(genDTO, materialIndex,
  materialID, properties, fluorescenceResults) core (the LambertianMulti
  overload delegates to it + adds nodeMultiplicativeFactor handling). This
  fixes the emissivity padding (extra Q,U,V / PSI,PSII bands now zero-padded
  via appendPerBandZeroPadded) and adds the bioClimaticFilePath key for
  fluorescent materials. Thermal/temperature keys dropped (EmissionMaterial).
  Foliar optical factors ("v<model>OF") still written in treatFoliar.
  NOTE for owner: "fluorescencefiles" now comes from
  FluorescenceResults.fileName (includes ".txt"; the old foliar block wrote
  "fluorescence_5_<model>_<face>" WITHOUT the extension) and the material
  "id" key is now written (old block had none) with the equivalent index.
  PhaseDAO::readTemperatureProperties stays dropped (never implemented).
- FOLIAR PROSPECT GENERATION: DONE (see history item 11). prepareProspect()
  covers surface lambertians AND vegetation (top/bottom faces) from the
  phsdata structs; no more PhaseDAO reads or static maps on this path.
- PER-MODEL DIRECTION SETS: DONE (see history item 13). VegetationMaterialWriter
  loops the UnderstoryMulti models grouped by DirectionSetLink and redoes the
  direction-set-dependent precomputing only on change; output model index is
  UnderstoryMulti::indexFunction.
- Port the remaining foliar PhaseDAO reads into phsdata::UnderstoryMulti:
  readCoeffDiffParametersForFoliar (LAD params) DONE (item 19);
  readCoeffDiffParametersForFoliarOpticalFactors DONE (item 20);
  readMultiplicativeFactorFoliar DONE (item 22).
- Remove the redundant per-band LAD recompute inside
  PhasePolarization::computeAndWriteFoliarPolarizationMatrix{,NotBinary}:
  DONE (history item 18).
- Rename the identifier useBioCLimaticProfileEta -> useBioClimaticProfileEta
  (lowercase l): DONE (history item 15).
- NetCDF/HDF5 atmosphere path: build netcdf-c + netcdf-cxx4 + hdf5 into the
  prefix and flip DART_WITH_NETCDF to finish gating work.
- CRLF normalization (7 files).
- Boost→std migration plan (agreed): shared_ptr/optional/lexical_cast/
  algorithm-string → std; regex→std::regex; filesystem→std::filesystem;
  thread→std::thread; date_time→std::chrono; uuid stays (header-only) for now.
- Old-style leftovers in Phase.cpp (3700 lines, ex-Phase.c): magic numbers
  TYPE_MODELISATION_* defines, French/English mix, commented-out log4cplus.
- librairieDart files NOT in dart_core yet (libIlwis, MakInversionParser,
  netcdf, yaml, lut/inversion databases, JobWriter): add when a module
  needs them. MakInversionParser still uses TinyXML (needs port to pugixml).
- FOLIAR C-STRUCTURE CLEANUP: DONE (item 23). Single-slot per-model storage
  in FoliarModel/FoliarComputation; per-model data computed in treat() only.
  (Reference note: the single-slot LAD vectors were item 16, commit a45629a;
  item 17 is the patch-transfer incident of that delivery.) Possible later
  iterations: equivalentFront/BackLambertian live in the slot although they
  are only used within treat() -- could become writer locals.
- REFACTOR INTO MaterialWriter CLASSES (same pattern as the Lambertian / Hapke
  / RPV writers): treatSpecular DONE (item 29, SpecularMaterialWriter); remaining:
  treatPhaseExtern (Phase.cpp), treatMixedMaterial (Phase.cpp).
- DEAD CODE REMOVAL: treatHapke (definition PhaseSurfaces.cpp:10, declaration
  Phase.hpp:305) has NO callers -- Hapke1981MaterialWriter replaced it;
  delete it (also check the treatHapke2 declaration, Phase.hpp:332), then
  sweep for other dead code.
- REMAINING PhaseDAO CALL SITES to port into phsdata / remove:
  readMultiplicativeFactorFoliar DONE (item 22; the last foliar DAO read);
  PhaseSurfaces.cpp:116 and Phase.cpp:1195 call
  readCoeffDiffParametersForLambertianOpticalFactors with the HAPKE /
  SPECULAR model index -- same pattern as the line-387 bug fixed in item 20;
  verify against the v5/v6 templates whether specular/hapke own an optical
  factor subtree in v6 or intentionally reuse the lambertian one;
  Phase.cpp:1768 readCoeffDiffParametersForFluidOpticalFactors
  (atmosphere / fluid path).
- DATABASE SELF-DESTRUCT not properly handled: the selfDestruct loop
  (Phase.cpp ~2234, over opticalPropertiesDatabases built by
  createDatabasesReadingProperties) must cover ALL temporary databases. After
  the writer refactor, lambertian prospect/marmit DBs get selfDestruct=true
  in LambertianMaterialWriter::prepareDatabase -- check they are also
  registered in the reading-properties list (gap noticed during the
  vegetation-writer session).
- FOLIAR OPTICAL FACTORS Top-vs-Bottom: the v5/v6 XMLs define optical
  factors for BOTH UnderstoryMultiTopModel and UnderstoryMultiBottomModel.
  The standard v<i>OF section currently uses Top only, and the
  equivalent-lambertian materials reuse the SAME foliar texture for
  consistency (item 20). Open question: what are the Bottom OFs for?
  Compare with PhaseOld/ and the templates; decide whether bottom-face
  outputs must use the Bottom list.
- PhaseOld/ (added in 9dc5a2f) = code of Phase before the factorization, kept
  for old-vs-new comparison and validation (bugs, unintended behavior
  changes, new behaviors to document).
- MODULE PROGRESS REPORT: setParametersForPrintingProgress() /
  checkProgress() (PhaseProperty.hpp:157/170) are a minimal cout-based
  approximate percentage progress report (_tModulo/_tIteration/_tToPercent).
  Currently UNCALLED (the only caller was removed from
  prepareVegetationDirectionSet in c8ca7fd); _tModulo defaults to 1 (item 28)
  so checkProgress() no longer divides by zero, but no progress is printed.
  Rewrite properly LATER, after all the processes themselves are completed.

## Pitfalls encountered (do not rediscover)

- Chat transfer of patches is lossy in TWO ways (see the "Patch transfer
  protocol" section at the end of this file):
  (a) receiver-side whitespace mangling: tabs in pasted plain patches become
      spaces or a literal "\t" (observed on the test patch committed in
      b708ea6: git am "Corrupt patch at line 59", git apply "corrupt patch at
      line 76"; the "\t" slip was fixed in 11a774d). Retry with
      `git am --ignore-whitespace`; `index` lines and blob shas in headers are
      not verified and may be hand-fixed. Variant observed on the per-model
      LAD patch (evidence: AICommunications repo, transfer/failed/, commit
      b1bfbe6): besides tab->space(s), some leading tabs are DELETED outright
      (38 of 353 damaged lines) -- still benign: no line-count change, so no
      "corrupt patch" error, and the tree differs only by whitespace (`git
      diff -w` vs the sender's commit is the gate).
  (b) sender-side character slips in long pastes, on ANY character, not just
      tabs -- plain patches included. Verified on eqLambertian.b64 (added in
      f65f439, removed in 39e3635): a 489-line base64 arrived with 3 harmful
      edits (lines 23 and 231: one substituted char each; line 379: one
      inserted char making 77 chars, so the stream length mod 4 = 1 and
      `base64 -d` fails with "invalid input") plus one harmless line split
      (lines 249-252). Two of the three harmful edits were SILENT: base64
      decoded fine, to wrong bytes. In a plain patch a slipped char usually
      fails loudly at apply time; in base64 it can pass silently. Rule:
      successful decode/apply proves nothing -- only a sha256 match does.
      Second incident (same day: the CONTEXT.md patch sent as 3 base64 chunks,
      receiver evidence in commit 416dcfb): one SUBSTITUTED char in chunk_aa
      line 48 corrupted a single 4-char quantum; the bad bytes landed in an
      ADDED diff line, so `git am` applied it silently (typo "`index`" ->
      "`index  " in the new text) and only the per-chunk sha256 flagged
      chunk_aa (ab/ac matched). Substitutions keep the stream aligned;
      insertions/deletions shift it (the dciIg 77-char line above).
  (c) editor/clipboard-side trailing-whitespace stripping on the RECEIVER
      side: whitespace-only lines lose their trailing space (" " context
      lines become truly empty, the "-- " git signature becomes "--").
      Verified with the protocol patch itself (receiver evidence in commit
      35d2120, transfer/failed/): the raw sha256 differed, yet the stripped
      patch applied and produced the IDENTICAL tree
      (220f8101d73bc884b2ee79a6b5763eea3ff7aa8f both ways), because git
      treats an empty line in a hunk as an empty context line. Benign for
      blank lines and the signature; dangerous if trailing spaces of
      NON-blank context lines get stripped (then use --ignore-whitespace).
  (d) the current chat transport DELETES lines whose post-marker content is
      empty/whitespace-only: an empty added line "+" and a single-space
      context line " " vanish entirely (a trailing blank line after the
      "-- " signature too). Unlike (c) this is STRUCTURAL: the hunk then has
      fewer lines than its @@ header claims and git fails with
      "corrupt patch at line N". Verified on the foliar-prospect patch
      (receiver evidence: transfer/failed/chunk_* in commits 09ca36a/ee376b6,
      applied in 965fa10): exactly 2 code lines dropped (my lines 406 "+"
      and 801 " "), while ALL added "+" lines with real content arrived
      byte-intact, and the tab mangling hit only context/removed lines
      (leading tabs -> 4-stop spaces; long tab runs after "-" shortened).
      Repair (validated): reinsert the dropped lines at their received-file
      positions with a 3-line python script, then
      `git apply --check --ignore-whitespace` + `git am --ignore-whitespace`;
      the post-am tree hash matched the sender's exactly
      (0a98de5222a12372f4f27e22b73f753bdc5f9a9b). One extra benign slip was
      observed: a char inserted in a diffstat header line (ignored by
      git apply). Note: plain `git apply --check` FAILS on the tab/space
      mangling of context lines even when the structure is fine -- always
      gate with --ignore-whitespace plus the tree hash.
  (e) second large patch (vegetation writer, 1334 lines; receiver evidence
      in transfer/failed/ at e4fd093/64fe6b0): additional STRUCTURAL modes
      on top of (a)-(d): a long context line full of tab runs SPLIT into two
      lines; one "-" removed line DROPPED; a junk char prefixed to an "@@"
      hunk header; a context-line marker flipped " " -> "-" (the line is then
      counted as removed); backslashes INSERTED before quotes inside removed
      lines; the trailing newline of the last chunk lost. Plus 32 added "+"
      lines with whitespace-only damage (owner accepted: the tree is not
      byte-exact; `git diff -w` against the sender's commit is the gate).
      Repair = a minimal line-ops python script (18 ops, descending line
      numbers, strings ASCII repr-escaped), validated in a scratch clone
      (`git am` OK, post-am tree 5f7c0e047d2b798e75c0b8ae9954f5314463cbf7,
      `git diff -w` vs b35bea3 empty), then applied by the owner as 015c6e8.
  (f) sender-side CASE FLIPS hit the transmitted REPAIR SCRIPT itself: the
      python patcher arrived with 8 case flips (useBioClimaticProfileEta
      instead of the code's useBioCLimaticProfileEta). `git apply --check
      --ignore-whitespace` PASSED but `git am` failed at Phase.cpp:1514 --
      repair scripts are as slip-prone as patches. Rule: never transmit a
      repair script (or any case-sensitive payload) as plain text; use the
      base64 path (protocol points 4/6) and gate on the sha256 of the
      DECODED file. Base64 is not immune either: a padding "=" was flipped
      to "-" in transit (hand-fixed by the receiver; the decoded sha256
      still mismatched due to further benign edits) -- again, only the
      decoded-output hash is the gate, decode/apply success proves nothing.
  (g) new variant observed on the FoliarModel-cleanup patch (receiver
      evidence: AICommunications commit e8cc536): DUPLICATED bare "-" lines
      (removed blank-line markers, post-marker content empty) INSERTED in a
      received chunk -- structural: the hunk then has more lines than its @@
      header claims -> "corrupt patch at line N" (observed at patch line 397 =
      chunk_03 line 97). Repair: delete the duplicated lines at their
      received-file positions (chunk_03 lines 56/76/80/84), validated by
      applying with --ignore-whitespace and an empty `git diff -w` against
      the sender's commit. Related: whitespace-only context lines dropped at
      chunk seams (chunk_08 line 100) are also structural.
- Sessions cannot push to GitHub (connector disabled). Delivery = patches per
  the protocol below; the owner pushes, sessions fetch anonymously.
- fresh CMake configure fails if find_package(Boost) has no COMPONENTS:
  Boost::filesystem etc. only exist when requested. Boost::lexical_cast does
  NOT exist as a target (header-only): use Boost::boost.
- SOCI 4.2 use() does not accept temporaries (dangling-ref protection).
- The sandbox has no root; no apt mirrors; build deps from github clones.
- Long/background builds die between tool calls; keep builds in one command.

## Patch transfer protocol (no push access) — verified 2026-11-18, revised same day

Corruption modes (see Pitfalls): receiver-side whitespace mangling of plain
patches, and sender-side character slips in long pastes. Base64 slips have two
shapes: a SUBSTITUTION keeps the stream aligned and corrupts only its own
4-char quantum (up to 3 bytes) -- it can even apply silently if the bad bytes
land in an added diff line (observed: CONTEXT.md patch chunk_aa, commit
416dcfb), so only the hash catches it; an INSERTION/DELETION shifts the whole
stream (the dciIg 77-char line of eqLambertian.b64 -> "invalid input").
Plain-text patch slips fail LOUDLY ("corrupt patch at line N"), and structured
text is emitted more reliably than base64 -- so plain text is the default:

1. Prefer PLAIN-TEXT patches, kept small: split one logical change into
   several patches of <= ~100 lines each (slip probability grows with payload
   length), and send the chunks in batches of AT MOST 4 chunks per message
   (session memory limits).
2. Every patch travels with its `sha256sum` AND the sender's post-am tree
   hash (`git rev-parse HEAD^{tree}` after am, or `git ls-tree HEAD -- <file>`
   blob shas). Receiver gates, in order: (a) `git apply --check` (loud
   structural failure); (b) `git am`; (c) AUTHORITATIVE: compare the tree/blob
   hash -- only a match proves the content arrived intact, because it is
   immune to trailing-whitespace stripping (pitfall c) and to commit-message
   edits. The raw file sha256 is a DIFFERENCE DETECTOR, not a pass/fail gate
   (benign whitespace-strips change it). On tab mangling,
   `git am --ignore-whitespace` (index/blob-sha lines are not verified and
   may be hand-fixed).
3. Repair loop on ANY gate failure or raw-sha mismatch: the receiver pushes
   the file as received into `transfer/failed/` of the DEDICATED transfer
   repo https://github.com/NicolasHHLAURET/AICommunications.git (anonymous
   clone; keeps the Phase repo clean); the sender fetches it anonymously,
   diffs locally against the true file, and classifies
   the difference (benign whitespace-strip vs harmful character slip). If
   harmful, the sender sends back ONLY a minimal correction -- the differing
   lines by number, each with its own sha256, or a short python script --
   never the whole payload. Final check: the post-am tree/blob hash.
4. Base64 fallback only for whitespace-critical payloads (tabs inside string
   literals, ...) or after repeated plain-text failures: `base64 -w 76`, then
   chunks of <= 50 lines (`split -l 50`), one code block per chunk, each
   preceded by its `sha256sum`; re-send only the failed chunk(s); reassemble
   `cat chunk_* > p.b64`; authoritative check `base64 -d p.b64 | sha256sum`
   == sender's decoded-patch sha256 (the whole-file hash may differ by a
   trailing newline). Decode/apply success proves nothing -- only the hash.
5. Validated plain-text repair loop (2026-11-19, foliar patch, evidence in
   transfer/failed/ + commit 965fa10; since 2026-11-23 failed files go to the
   AICommunications repo, e.g. commit b1bfbe6 for the per-model LAD patch):
   receiver compares PER-CHUNK sha256
   first (cheap, localizes the damage), pushes received chunks to
   transfer/failed/, sender fetches and diffs against the true chunks,
   classifies each difference (benign tab/space mangling on context/removed
   lines and in diffstat headers vs harmful drops/substitutions). For
   pitfall (d) drops the correction is a tiny script reinserting the missing
   lines at received-file positions -- never resend the whole payload. Apply
   with `git am --ignore-whitespace` (plain --check is expected to fail on
   tab mangling) and gate on the post-am tree hash. For tab-indented C++
   patches with many added lines, plain text + this repair loop worked, but
   going straight to base64 (point 4) is legitimate when the added lines are
   whitespace-critical.
6. Repair scripts and other case-sensitive payloads (pitfall f): transmit
   them base64-encoded (point 4 mechanics: `base64 -w 76`, chunks of
   <= 50 lines, one code block per chunk with its sha256), reassemble
   `cat chunk_* > p.b64`, and gate on `sha256sum` of the DECODED file
   (`base64 -d p.b64 | sha256sum`) against the sender's value -- never on
   decode/apply success. Expect "=" padding flips to "-" and silent
   substitutions; only the hash catches them.
7. Receiver checklist, once all chunks are received (owner side):
     cat chunk_00 chunk_01 ... chunk_NN > p.patch        # in order
     sha256sum chunk_*                                   # difference detector vs sender's per-chunk sha256
     git apply --check --ignore-whitespace p.patch       # loud structural gate (plain --check fails on tab mangling)
     git -c user.name=... -c user.email=... am --ignore-whitespace p.patch
     git rev-parse HEAD^{tree}                           # AUTHORITATIVE: must equal the sender's post-am tree hash
   If the tree hash differs but `git diff -w <applied-commit> <sender-commit>`
   is empty, the difference is whitespace-only on added lines (benign, item
   14/23 precedent) -- accepted by the owner with diff -w as the gate.
   On ANY failure: push the received files (chunks + p.patch) to
   transfer/failed/ of the AICommunications repo, then follow point 3/5
   (sender sends back only a minimal correction, base64 + decoded-sha256
   gate per point 6).
8. Corrections fix ONLY harmful errors (dropped/duplicated/substituted
   lines, stray characters, line-count breaks). Non-harmful
   whitespace/tabulation mangling (tabs<->spaces on context/removed lines,
   in diffstat headers, or on added lines) is NEVER corrected: it is
   covered by `git apply/am --ignore-whitespace` and by the tree-hash /
   `git diff -w` gates. Correcting it only bloats the correction payload
   and invites new slips (observed 2026-11-26, specular patch: a
   byte-identical-rebuild repair script was 5.2 KB; the harmful-only fix
   was a 0.5 KB two-line insertion).

## Session handoff checklist (for AI agents)

1. `git fetch origin` (anonymous fetch works; push does NOT -- no connector,
   org/workspace policy blocks it). Read this file end-to-end, then
   `git log --oneline -10`.
2. Work in an anonymous clone of https://github.com/NicolasHHLAURET/Phase
   (e.g. /workspace/Phase). The GitHub connector cannot be enabled from a
   session; do not try ask_enable_connector (only pubmed is listed).
3. Commit locally as `Vibe <vibe@localhost>` -- the owner uses this identity
   to tell AI-made commits apart from his own.
4. Deliver changes as `git format-patch` plain-text chunks of <= ~100 lines
   (one code block per chunk), sent in batches of at most 4 chunks per
   message, each chunk with its sha256, plus the full patch sha256 and the
   post-am tree hash; give the `cat chunk_* > p.patch` reassembly command
   (see protocol point 7 for the full receiver checklist). On any
   receiver-side failure, expect the received
   files in transfer/failed/ of the AICommunications repo (clone
   https://github.com/NicolasHHLAURET/AICommunications.git) and follow
   protocol point 3/5.
5. Update this file at the end of each session (history item + TODOs +
   pitfalls); it is the only handoff between sessions.
6. Build only if needed: no DART_DEPS_PREFIX in a fresh sandbox; see "Build
   state" for rebuilding it (foreground single command, no background).
7. DIAGNOSTIC FIRST (owner rule, 2026-11-26): for multi-step tasks, do the
   diagnostic first and END IT by asking the owner for the go-ahead before
   writing any code. One-shot implementation of a multi-step task is
   error-prone; the owner validates the diagnostic, then says proceed.
8. SANDBOX IS EPHEMERAL: /workspace and /tmp were observed wiped mid-session
   (2026-11-26) while network access stayed up. The pushed git repo is the
   only durable state: commit every change locally (identity
   `Vibe <vibe@localhost>`) so any deliverable can be regenerated with
   `git format-patch`; do not rely on /tmp artifacts (stubs, chunks, repair
   scripts) surviving between messages.
