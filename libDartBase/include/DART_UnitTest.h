#ifndef _DART_UNITTEST_H_
#define _DART_UNITTEST_H_
/*! Time-stamp: <@(#) DART_UnitTest.h   10/09/04 - 10:00:50   GPR MAGELLIUM>
 *********************************************************************
 *  @file    : DART_UnitTest.h
 *
 *  Projet   : DART
 *
 *  Societe  : MAGELLIUM
 *
 *  Auteur   : GPR                               Date: 07/03/2005
 *
 *  Description : MACRO de trace des Tests Unitaires
 * 
 *  Utilisation : 
 * 
 *  1/Dans chaque .C du module � tester inclue DART_UnitTest.h
 *  2/Dans le .C du main appeler la macro DART_UT_TRACE_MAIN(nom_du_modul) 
 *  3/Appeler DART_UT_TRACE_INIT() en d�but du main
 *  4/Appeler DART_UT_TRACE_EXIT() en sortie du module
 *
 *********************************************************************
 * Historique :
 *
 * V 4.3.0 12/03/2007  MAG-EMN : Livraison
 * V 4.2.2 28/07/2006  CLF : Livraison
 * V 4.2.0 05/04/2006  CLF : Livraison
 * V 4.1.1 23/12/2005  CLF : Livraison
 * V 4.1.0 03/11/2005  VBT : Livraison 
 * V 1.0  12/04/2005  GPR : First Revision
 *
 *********************************************************************
 */

#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include <fstream>

#ifdef DART_UT_TRACE

extern ofstream g_DartUTTraceStream;

#define DART_UT_TRACE_MAIN(modulname) \
string _smodulname( modulname) ;\
ofstream g_DartUTTraceStream;\

#define DART_UT_TRACE_IN(_fuctionname,_stream) \
    g_DartUTTraceStream<<"DART_UT_TRACE  IN ["<< _fuctionname << "]\t\t\t : "<< _stream << endl ;

#define DART_UT_TRACE_OUT(_fuctionname,_stream) \
    g_DartUTTraceStream<<"DART_UT_TRACE OUT ["<< _fuctionname << "]\t\t\t : "<< _stream << endl ;

#define DART_UT_TRACE_INIT() \
    string sfilename =   _smodulname+ string("_ut")+ string(".log") ;\
    g_DartUTTraceStream.open(sfilename.c_str()) ; \
    cout << "DART TEST UNIT ON on file : ["<<sfilename.c_str()<<"]"<<endl;

#define DART_UT_TRACE_EXIT() \
    g_DartUTTraceStream.close();

#else // DART_UT_TRACE

#define DART_UT_TRACE_MAIN(modulname)
#define DART_UT_TRACE_IN(_fuctionname,_stream)
#define DART_UT_TRACE_OUT(_fuctionname,_stream)
#define DART_UT_TRACE_INIT()
#define DART_UT_TRACE_EXIT()

#endif

#endif //_DART_UNITTEST_H_
