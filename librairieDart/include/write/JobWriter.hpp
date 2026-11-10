/*
 * WritingJob.hpp
 *
 *  Created on: 19 avr. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_CLASSES_PRODUITSDART_WRITE_JOBWRITER_HPP_
#define INCLUDE_CLASSES_PRODUITSDART_WRITE_JOBWRITER_HPP_

/*
 * Interface des jobs d'écriture gérés par le WritingManager
 */
class JobWriter {
private:
public:
	virtual ~JobWriter() {}

	/**
	 * Excecute le job d'écriture
	 */
	virtual void write() = 0 ;
};

#endif /* INCLUDE_CLASSES_PRODUITSDART_WRITE_JOBWRITER_HPP_ */
