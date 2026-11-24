/*
 * MarmitComputation.hpp
 *
 *  Created on: 14 févr. 2022
 *      Author: guilleuxj
 */

#ifndef INCLUDE_PHASEEXTERN_MARMITCOMPUTATION_HPP_
#define INCLUDE_PHASEEXTERN_MARMITCOMPUTATION_HPP_

#include <boost/algorithm/string/replace.hpp>
using namespace std;

class MarmitComputation {
private:
	double _smc;
	double _delta;
	std::string _modelName;
	std::string _dbName;
public:
	MarmitComputation(const double & smc, const double & delta, const std::string & modelName, const std::string & dbName) :
		_smc(smc), _delta(delta), _modelName(modelName), _dbName(dbName) {
	}

	~MarmitComputation() {}

	void runMarmitDatabaseGeneration() {
		ostringstream commandLineRoot;
		commandLineRoot << DartEnv::getPythonExecutable() << " ";
		commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "DART_MARMIT" << pathSeparator << "DART_MARMIT.py" << " ";
		commandLineRoot << "-mode db" << " ";
		commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" <<pathSeparator << "DARTTools" <<  pathSeparator << "DART_MARMIT" << " ";
		commandLineRoot << DartEnv::getLocalTempDatabasePath() << _dbName << " ";
		commandLineRoot << "\"'" << _modelName << "'\"" << " ";
		commandLineRoot << _smc << " ";
		commandLineRoot << _delta << " ";
		std::cout << "    Run python cmd: " << commandLineRoot.str() << endl;
		std::system(commandLineRoot.str().c_str());
	}

	void runMarmitOutputFileGeneration() {
		ostringstream fileName;
		fileName << boost::replace_all_copy(boost::replace_all_copy(_modelName, " ", "_"), "::", "_");
		fileName << "_SMC_" << _smc;
		fileName << "_Delta_" << _delta;
		fileName << "_" << ".txt";
		ostringstream commandLineRoot;
		commandLineRoot << DartEnv::getPythonExecutable() << " ";
		commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" <<  pathSeparator << "DART_MARMIT" << pathSeparator << "DART_MARMIT.py" << " ";
		commandLineRoot << "-mode run" << " ";
		commandLineRoot << DartEnv::getHomePath() << "bin" << pathSeparator << "python_script" << pathSeparator << "DARTTools" << pathSeparator << "DART_MARMIT" << " ";
		commandLineRoot << DartEnv::getLocalHomePath() << "database" << pathSeparator << fileName.str() << " ";
		commandLineRoot << "\"'" << _modelName << "'\"" << " ";
		commandLineRoot << _smc << " ";
		commandLineRoot << _delta << " ";
		std::cout << "    Run python cmd: " << commandLineRoot.str() << endl;
		std::system(commandLineRoot.str().c_str());
	}
};

#endif /* INCLUDE_PHASEEXTERN_MARMITCOMPUTATION_HPP_ */
