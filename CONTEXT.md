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
   testIfUseSun implemented as: sun spectral irradiance for every band whose
   spectralDomain != PhaseGeneralParametersDTO::Thermal. OWNER: confirm.
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
  still to do: readMultiplicativeFactorFoliar
  (UnderstoryNodeMultiplicativeFactorForLUT -- same subtree as the optical
  factors, can reuse phsdata::UnderstoryMultiplicativeFactorList).
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
- FOLIAR C-STRUCTURE CLEANUP: continue cleaning the old C Foliar storage
  (FoliarModel / FoliarComputation / LADComputation): separate
  common-to-all-models data from per-model data (single-slot LAD vectors
  already done, history item 17), compute/allocate per-model data only when
  the model is treated, and drop it afterwards. Diagnostic (item 17): no
  cross-model access exists, except one unintended case where the XML
  indexFunction was used for data access instead of the loop position --
  fixed by the foliarIndice/outputModelIndex split.
- REFACTOR INTO MaterialWriter CLASSES (same pattern as the Lambertian / Hapke
  / RPV writers): treatSpecular (Phase.cpp:1031), treatPhaseExtern
  (Phase.cpp:1356), treatMixedMaterial (Phase.cpp:1423).
- DEAD CODE REMOVAL: treatHapke (definition PhaseSurfaces.cpp:10, declaration
  Phase.hpp:305) has NO callers -- Hapke1981MaterialWriter replaced it;
  delete it (also check the treatHapke2 declaration, Phase.hpp:332), then
  sweep for other dead code.
- REMAINING PhaseDAO CALL SITES to port into phsdata / remove:
  Phase.cpp:1003 readMultiplicativeFactorFoliar (the last foliar DAO read;
  note it also snaps iterationStepTheta/Phi to divisor tables -- preserve
  that when porting; it reads BOTH Top and Bottom models via
  readFoliarMultiplicativeFactorForModelXML, reusable through
  phsdata::UnderstoryMultiplicativeFactorList);
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
   length).
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
   (one code block per chunk), each chunk with its sha256, plus the full
   patch sha256 and the post-am tree hash; give the `cat chunk_* > p.patch`
   reassembly command. On any receiver-side failure, expect the received
   files in transfer/failed/ of the AICommunications repo (clone
   https://github.com/NicolasHHLAURET/AICommunications.git) and follow
   protocol point 3/5.
5. Update this file at the end of each session (history item + TODOs +
   pitfalls); it is the only handoff between sessions.
6. Build only if needed: no DART_DEPS_PREFIX in a fresh sandbox; see "Build
   state" for rebuilding it (foreground single command, no background).
