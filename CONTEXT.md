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
- Per-model direction sets (see item 9).
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

## Pitfalls encountered (do not rediscover)

- Chat transfer of patches is lossy in TWO ways (see the "Patch transfer
  protocol" section at the end of this file):
  (a) receiver-side whitespace mangling: tabs in pasted plain patches become
      spaces or a literal "\t" (observed on the test patch committed in
      b708ea6: git am "Corrupt patch at line 59", git apply "corrupt patch at
      line 76"; the "\t" slip was fixed in 11a774d). Retry with
      `git am --ignore-whitespace`; `index` lines and blob shas in headers are
      not verified and may be hand-fixed.
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
   the file as received into `transfer/failed/` on the repo; the sender
   fetches it anonymously, diffs locally against the true file, and classifies
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
