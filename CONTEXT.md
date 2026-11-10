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
- Patch-based delivery: the assistant cannot push (no GitHub connector),
  so changes are delivered as `git format-patch` output pasted in chat and
  applied with `git am` (use `git am --ignore-whitespace` if tabs get mangled;
  the `index <sha>..<sha>` lines are informational and may be hand-fixed).

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

## Known TODOs / open points (next sessions)

- Equivalent-lambertian from vegetation: writeLambertian and
  PhaseDAO::readTemperatureProperties were never implemented (any commit);
  calls dropped, marked TODO in Phase.cpp treatFoliar.
- Prospect generation for foliar models is commented out in prepareProspect;
  mapPrepareProspectFoliarTop/Bottom are static empty maps (consumer branch
  is dead until this is finished).
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

- git format-patch pasted in chat can get mangled (tabs). If git am fails on
  whitespace, retry with --ignore-whitespace; blob shas in headers are not
  verified and may be edited by hand.
- Sessions cannot push to GitHub (connector disabled). Delivery = patches.
- fresh CMake configure fails if find_package(Boost) has no COMPONENTS:
  Boost::filesystem etc. only exist when requested. Boost::lexical_cast does
  NOT exist as a target (header-only): use Boost::boost.
- SOCI 4.2 use() does not accept temporaries (dangling-ref protection).
- The sandbox has no root; no apt mirrors; build deps from github clones.
- Long/background builds die between tool calls; keep builds in one command.
