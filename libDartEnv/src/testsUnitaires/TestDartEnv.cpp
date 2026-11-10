/*
 * TestDartEnv.cpp
 *
 *  Created on: 9 févr. 2011
 *      Author: cortir
 */

#define BOOST_TEST_MODULE testClasseExempleLibBoost
#include <boost/test/unit_test.hpp>

#include "../../../libDartEnv/include/DartEnv.h"
#include "../../../libDartBase/include/GeneralDef.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE (nettoyerNomDeSimulation)

// Cas de test dit "standard" (forme a laquelle on s'attend, a priori la plus courante, donc pas de "piege")
BOOST_AUTO_TEST_CASE(testNomDeSimulationStandard)
{	
	const string chaineBrute1 = string(".") + pathSeparator + string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14");
	const string chaineBrute2 = string(".") + pathSeparator + string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator
			+ string("_reflec_spectral_2_2");
	const string NomDeSimulationAttendu1 = "LutSpectral";
	const string NomDeSimulationAttendu2 = "_reflec_spectral_";
	
	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse des informations de chemin diverses)
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu1, DartEnv::cleanSimulationName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu2, DartEnv::cleanSimulationName(chaineBrute2));
}

// Cas de test ou la chaine d'entree est deja dans la forme sous laquelle on souhaite la retourner
BOOST_AUTO_TEST_CASE(testNomDeSimulationDejaPropre)
{	
	const string chaine1 = "LutSpectral";
	const string chaine2 = "_reflec_spectral_";
	
	// Comportement souhaite : on doit retrouver le meme nom de simulation (puisqu'il etait deja "propre")
	BOOST_CHECK_EQUAL(chaine1, DartEnv::cleanSimulationName(chaine1));
	BOOST_CHECK_EQUAL(chaine2, DartEnv::cleanSimulationName(chaine2));
}

// Cas de test ou la chaine d'entree n'est que partiellement "encombree" via un prefixe (nota: on n'attend comme prefixe eventuel que la possibilite d'avoir "./")
BOOST_AUTO_TEST_CASE(testPrefixeNomDeSimulation)
{	
	const string chaineBrute1 = string(".") + pathSeparator + string("LutSpectral");
	const string chaineBrute2 = string(".") + pathSeparator + string("_reflec_spectral_");
	const string NomDeSimulationAttendu1 = "LutSpectral";
	const string NomDeSimulationAttendu2 = "_reflec_spectral_";
	
	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse du prefixe)
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu1, DartEnv::cleanSimulationName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu2, DartEnv::cleanSimulationName(chaineBrute2));
}

// Cas de test ou la chaine d'entree n'est que partiellement "encombree" via un suffixe (nota: on n'attend comme suffixe que la possibilite d'avoir un chemin)
BOOST_AUTO_TEST_CASE(testSuffixeNomDeSimulation)
{	
	const string chaineBrute1 = string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14");
	const string chaineBrute2 = string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator + string("_reflec_spectral_2_2");
	const string NomDeSimulationAttendu1 = "LutSpectral";
	const string NomDeSimulationAttendu2 = "_reflec_spectral_";
	
	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse du suffixe)
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu1, DartEnv::cleanSimulationName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSimulationAttendu2, DartEnv::cleanSimulationName(chaineBrute2));
}

// Cas de test ou l'on fournit a la methode une chaine vide
BOOST_AUTO_TEST_CASE(testNomDeSimulationVide)
{	
	const string chaineVide = "";

	// Comportement souhaite : on attend une chaine vide en retour
	BOOST_CHECK_EQUAL(chaineVide, DartEnv::cleanSimulationName(chaineVide));
}

// Cas de test ou un délimiteur se trouve en fin de chaine contrairement au formes usuelles de getCurrentsimulationName() sur lesquelles est basee la methode testee
BOOST_AUTO_TEST_CASE(testNomDeSimulationAvecDelimiteurFinal)
{	
	const string chaineBrute1 = string(".") + pathSeparator + string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14") + pathSeparator;
	const string chaineBrute2 = string(".") + pathSeparator + string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator
			+ string("_reflec_spectral_2_2") + pathSeparator;
	const string chaineBrute3 = string(".") + pathSeparator + string("LutSpectral") + pathSeparator;
	const string chaineBrute4 = string(".") + pathSeparator + string("_reflec_spectral_") + pathSeparator;
	const string chaineBrute5 = string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14") + pathSeparator;
	const string chaineBrute6 = string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator + string("_reflec_spectral_2_2")
			+ pathSeparator;
	const string NomDeSimulationAttenduImpair = "LutSpectral";
	const string NomDeSimulationAttenduPair = "_reflec_spectral_";
	
	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse des informations de chemin diverses)
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduImpair, DartEnv::cleanSimulationName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduPair, DartEnv::cleanSimulationName(chaineBrute2));
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduImpair, DartEnv::cleanSimulationName(chaineBrute3));
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduPair, DartEnv::cleanSimulationName(chaineBrute4));
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduImpair, DartEnv::cleanSimulationName(chaineBrute5));
	BOOST_CHECK_EQUAL(NomDeSimulationAttenduPair, DartEnv::cleanSimulationName(chaineBrute6));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(nettoyerNomDeSequence)

// Cas de test dit "standard" (forme a laquelle on s'attend, a priori la plus courante, donc pas de "piege")
BOOST_AUTO_TEST_CASE(testNomDeSequenceStandard)
{	
	const string chaineBrute1 = string(".") + pathSeparator + string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14");
	const string chaineBrute2 = string(".") + pathSeparator + string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator + string("_reflec_spectral_2_2");
	const string NomDeSequenceAttendu1 = "sol_bande";
	const string NomDeSequenceAttendu2 = "_reflec_spectral_2";

	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse des informations de chemin diverses)
	BOOST_CHECK_EQUAL(NomDeSequenceAttendu1, DartEnv::cleanSequenceName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSequenceAttendu2, DartEnv::cleanSequenceName(chaineBrute2));
}

// Cas de test ou la chaine d'entree est deja dans la forme sous laquelle on souhaite la retourner
BOOST_AUTO_TEST_CASE(testNomDeSequenceDejaPropre)
{	
	const string chaine1 = "sol_bande";
	const string chaine2 = "_reflec_spectral_2";
	
	// Comportement souhaite : on doit retrouver le meme nom de sequence
	BOOST_CHECK_EQUAL(chaine1, DartEnv::cleanSequenceName(chaine1));
	BOOST_CHECK_EQUAL(chaine2, DartEnv::cleanSequenceName(chaine2));
}

// Cas de test ou l'on fournit a la methode une chaine vide
BOOST_AUTO_TEST_CASE(testNomDeSequenceVide)
{	
	const string chaineVide = "";

	// Comportement souhaite : on attend une chaine vide en retour
	BOOST_CHECK_EQUAL(chaineVide, DartEnv::cleanSequenceName(chaineVide));
}

// Cas de test ou un délimiteur se trouve en fin de chaine contrairement au formes usuelles de getCurrentsimulationName() sur lesquelles est basee la methode testee
BOOST_AUTO_TEST_CASE(testNomDeSequenceAvecDelimiteurFinal)
{	
	const string chaineBrute1 = string(".") + pathSeparator + string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14") + pathSeparator;
	const string chaineBrute2 = string(".") + pathSeparator + string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator
			+ string("_reflec_spectral_2_2") + pathSeparator;
	const string chaineBrute3 = string("LutSpectral") + pathSeparator + string("sequence") + pathSeparator + string("sol_bande_14") + pathSeparator;
	const string chaineBrute4 = string("_reflec_spectral_") + pathSeparator + string("sequence") + pathSeparator + string("_reflec_spectral_2_2")
			+ pathSeparator;
	const string NomDeSequenceAttenduImpair = "sol_bande";
	const string NomDeSequenceAttenduPair = "_reflec_spectral_2";
	
	// Comportement souhaite : on ne doit retrouver que le nom de la simulation (debarasse des informations de chemin diverses)
	BOOST_CHECK_EQUAL(NomDeSequenceAttenduImpair, DartEnv::cleanSequenceName(chaineBrute1));
	BOOST_CHECK_EQUAL(NomDeSequenceAttenduPair, DartEnv::cleanSequenceName(chaineBrute2));
	BOOST_CHECK_EQUAL(NomDeSequenceAttenduImpair, DartEnv::cleanSequenceName(chaineBrute3));
	BOOST_CHECK_EQUAL(NomDeSequenceAttenduPair, DartEnv::cleanSequenceName(chaineBrute4));
}

BOOST_AUTO_TEST_SUITE_END()

