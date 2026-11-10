/*
 * WritingManager.hpp
 *
 *  Created on: 19 avr. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_CLASSES_PRODUITSDART_WRITE_WRITERMANAGER_HPP_
#define INCLUDE_CLASSES_PRODUITSDART_WRITE_WRITERMANAGER_HPP_

#include <boost/lexical_cast.hpp>
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
#include <unordered_map>
using namespace std;
#else
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

#include "JobWriter.hpp"
#include "../SynchronisedQueue.hpp"

/*
 * Gère les jobs d'écriture de DART.
 */
class WriterManager {

private:
	SynchronisedQueue<shared_ptr<JobWriter> > _jobQueue;
	unsigned int _nbThreads;
	unsigned int _nbWorkers;
	bool _isJoining;
	boost::mutex _workersMutex;
	boost::thread_group _workersGroup;

	/**
	 * Ajoute un travailleur à la collection si le nombre de travailleurs
	 * est inférieur au nombre maximum de threads possible.
	 */
	void _addWorker() {
		if (_nbWorkers < _nbThreads && !_isJoining) {
			_nbWorkers++;
			boost::thread* t = new boost::thread(boost::bind(&WriterManager::_workerRun, this));
			_workersGroup.add_thread(t);
		}
	}

	/**
	 * Travailleur : Lance un travail d'écriture si disponible, se tue sinon.
	 * Lorsque le travail d'écriture est terminé, tente d'en relancer un.
	 */
	void _workerRun() {
		_workersMutex.lock();
		if (_jobQueue.getSize() > 0) {
			shared_ptr<JobWriter> job = _jobQueue.dequeue();
			_workersMutex.unlock();
			job->write();
			this->_workerRun();
		} else {
			_nbWorkers--;
			_workersMutex.unlock();
		}
	}

public:
	~WriterManager() {
		this->waitWriting();
	}

	WriterManager() :
			_nbThreads(1), _nbWorkers(0), _isJoining(false) {}

	/**
	 * Met à jour le nombre de threads disponible (maximum de travailleurs).
	 */
	void setNumberOfThreads(const unsigned int nbThreads) {
		_nbThreads = nbThreads > 0 ? nbThreads : 1;
	}

	/**
	 * Ajoute un travail d'écriture.
	 */
	void addJob(std::shared_ptr<JobWriter> job) {
		_workersMutex.lock();
		_jobQueue.enqueue(job);
		this->_addWorker();
		_workersMutex.unlock();
	}

	/**
	 * Attends la terminaison des écritures. (càd. aucun travailleurs en cours d'execution).
	 */
	void waitWriting() {
		_isJoining = true;
		_workersGroup.join_all();
		_isJoining = false;
	}

};
#endif /* INCLUDE_CLASSES_PRODUITSDART_WRITE_WRITERMANAGER_HPP_ */
