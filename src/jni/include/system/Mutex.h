/*
 * Mutex.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Aug 30, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _SYSTEM_MUTEX_H_
#define _SYSTEM_MUTEX_H_

#include <pthread.h>

/**
 * @brief Lock
 */
class Mutex {
public:
	enum {
		E_TYPE_PRIVATE = 0,
		E_TYPE_SHARED = 1
	};

	Mutex();
	Mutex(int type);
	~Mutex();

	/**
	 * @brief Locks the mutex
	 */
	void lock();

	/**
	 * @brief Unlocks the mutex
	 */
	void unlock();

	void tryLock();

	/**
	 * @brief Automatic lock
	 */
	class Autolock {
	public:
	  /**
	   * @brief Locks the mutex upon construction
	   */
	  inline Autolock(Mutex& mutex) : mLock(mutex) { mLock.lock(); }

	  /**
	   * @brief Locks the mutex upon construction
	   */
	  inline Autolock(Mutex* mutex) : mLock(*mutex) { mLock.lock(); }

	  /**
	   * @brief Unlocks the mutex upon destruction
	   */
	  inline ~Autolock() { mLock.unlock(); }

	private:
		Mutex &mLock;
	};

private:
	friend class Condition;

	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

	pthread_mutex_t mMutex;
};

#endif /* _SYSTEM_MUTEX_H_ */
