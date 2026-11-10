/*
 * SynchronisedQueue.hpp
 *
 *  Created on: Aug 5, 2015
 *      Author: yint
 */

#ifndef INCLUDE_CLASSES_MONTECARLO_SYNCHRONISEDQUEUE_HPP_
#define INCLUDE_CLASSES_MONTECARLO_SYNCHRONISEDQUEUE_HPP_

#include <boost/thread.hpp>
#include <fstream>
#include <queue>

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
using namespace std;
#else
#include <boost/shared_ptr.hpp>
using namespace boost;
#endif

template<typename T>
class SynchronisedQueue {
private:
	std::queue<T> m_queue;
	boost::mutex m_mutex;
	boost::condition_variable m_cond;

public:
	void enqueue(T const& data) {
		boost::unique_lock<boost::mutex> lock(m_mutex);
		m_queue.push(data);
		m_cond.notify_one();
	}
	T dequeue() {
		boost::unique_lock<boost::mutex> lock(m_mutex);
		while (m_queue.size() == 0)
			m_cond.wait(lock);
		T result = m_queue.front();
		m_queue.pop();
		return result;
	}
	unsigned int getSize() {
		return m_queue.size();
	}
};

#endif /* INCLUDE_CLASSES_MONTECARLO_SYNCHRONISEDQUEUE_HPP_ */
